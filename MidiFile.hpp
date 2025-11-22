#ifndef MIDIFILE_HPP
#define MIDIFILE_HPP
#include"MidiHead.hpp"
#include"MidiTrack.hpp"
#include<string>
#include<utility>

namespace GoldType{
    namespace MidiParse{
        /********************************
                    Midi File
        ********************************/
        enum class MidiFileState:uint8_t{
            untouched,
            read_error,
            read_success,
            write_error,
            write_success
        };
        class MidiFile {
            protected:
                MidiFileState m_state;
            public:
                std::string filename;
                MidiHead head;
                MidiTrackList tracks;
            protected:
                uint32_t read_staticData(FILE*fin,size_t num){
                    uint32_t ret=0;
                    for(size_t i=0;i<num;++i){
                        ret=(ret<<8)+fgetc(fin);
                    }
                    return ret;
                }
                uint32_t read_dynamicData(FILE*fin) {
                    uint32_t ret=0;
                    for(size_t i=0;i<4; ++i) {
                        ret<<=7;
                        uint8_t a=fgetc(fin);
                        ret|=(a&0x7F);
                        if(a<0x80) {
                            return ret;
                        }
                    }
                    return ret;
                }
                uint32_t read_dynamicData(FILE*fin,uint32_t&varlen) {
                    uint32_t ret=0;
                    for(size_t i=0; i<4; ++i) {
                        ret<<=7;
                        uint8_t a=fgetc(fin);
                        ret|=(a&0x7F);
                        if(a<0x80) {
                            varlen+=i+1;
                            return ret;
                        }
                    }
                    varlen+=4;
                    return ret;
                }
                uint32_t read_dynamicData(FILE*fin,uint32_t&varlen,MidiMessage&message){
                    uint32_t ret=0;
                    for(size_t i=0; i<4; ++i) {
                        ret<<=7;
                        uint8_t a=fgetc(fin);
                        message.push_back(a);
                        ret|=(a&0x7F);
                        if(a<0x80) {
                            varlen+=i+1;
                            return ret;
                        }
                    }
                    varlen+=4;
                    return ret;
                }
                
            protected:
                MidiErrorType read_midiHead(FILE*fin){
                    if(!(fgetc(fin)=='M'&&fgetc(fin)=='T'&&fgetc(fin)=='h'&&fgetc(fin)=='d')){
                        midiError(MidiErrorType::head_identification);
                        return midiError.type();
                    }
                    if(read_staticData(fin,4)!=6){
                        midiError(MidiErrorType::head_length);
                        return midiError.type();
                    }
                    head.format=read_staticData(fin,2);
                    head.ntracks=read_staticData(fin,2);
                    head.division=read_staticData(fin,2);
                    midiError(head);
                    return midiError.type();
                }
                MidiErrorType read_midiTrack(FILE*fin){
                    tracks.resize(head.ntracks);
                    size_t event_count=0;
                    for(size_t trackIdx=0; trackIdx<head.ntracks; ++trackIdx) {
                        if(fgetc(fin)!='M'||fgetc(fin)!='T'||fgetc(fin)!='r'||fgetc(fin)!='k') {
                            midiError(MidiErrorType::track_identification);
                            return midiError.type();
                        }
                        uint32_t byte_num=read_staticData(fin,4);
                        uint32_t byte_read=0;
                        uint8_t last_eventType=(uint8_t)MidiEventType::null;
                        while(!feof(fin)) {
                            MidiEvent event;
                            event.time=read_dynamicData(fin,byte_read);
                            #ifdef MIDI_WARNING
                            if(event.time>0x0FFFFFFF){
                                midiError(MidiErrorType::event_deltaTime);
                                return midiError.type();
                            }
                            #endif
                            read_midiMessage(fin,last_eventType,event,byte_read);
                            if(midiError.type()!=MidiErrorType::noError) {
                                return midiError.type();
                            }
                            tracks[trackIdx].emplace_back(event);
                            ++event_count;
                            if(event.is_meta()&&event[1]==0x2F) {
                                break;
                            }
                        }
                        #ifdef MIDI_WARNING
                        if(byte_num!=byte_read){
                            midiError(MidiErrorType::event_deltaTime);
                            return midiError.type();
                        }
                        #endif
                    }
                    return midiError.type();
                }
                MidiErrorType read_midiMessage(FILE*fin,uint8_t&last_eventType,MidiEvent&event,uint32_t&byte_read) {
                    uint8_t now_eventType=fgetc(fin);
                    ++byte_read;
                    if(now_eventType<0x80) {
                        if(last_eventType==(uint8_t)MidiEventType::null||(last_eventType&0xF0)==0xF0) {
                            midiError(MidiErrorType::event_type);
                            return midiError.type();
                        } 
                        now_eventType=last_eventType;
                        fseek(fin,-1,SEEK_CUR);
                        --byte_read;
                    }
                    event.message.clear();
                    switch(now_eventType&0xF0) {
                        case 0x80:        //note_off
                        case 0x90:        //note_on
                        case 0xA0:        //key_afterTouch
                        case 0xB0:        //controller
                        case 0xE0: {      //pitchWheel
                            uint8_t data1=fgetc(fin);
                            uint8_t data2=fgetc(fin);
                            event.message={now_eventType,data1,data2};
                            byte_read+=2;
                            break;
                        }
                        case 0xC0:        //program
                        case 0xD0: {      //channel_afterTouch
                            uint8_t data1=fgetc(fin);
                            event.message={now_eventType,data1};
                            ++byte_read;
                            break;
                        }
                        case 0xF0: {      //sysex or meta
                            switch(now_eventType) {
                                case 0xFF: {    //meta
                                    event.message.push_back(now_eventType);
                                    uint8_t metaType=fgetc(fin);
                                    event.message.push_back(metaType);
                                    ++byte_read;
                                    uint32_t len=read_dynamicData(fin,byte_read,event.message);
                                    byte_read+=len;
                                    for(size_t i=0; i<len;++i){
                                        event.message.push_back(fgetc(fin));
                                    }
                                    break;
                                }
                                case 0xF0:        //sysex_begin
                                case 0xF7: {      //sysex_end
                                    event.message.push_back(now_eventType);
                                    uint32_t len=read_dynamicData(fin,byte_read,event.message);
                                    byte_read+=len;
                                    for(size_t i=0; i<len;++i){
                                        event.message.push_back(fgetc(fin));
                                    }
                                    break;
                                }
                                default: {
                                    midiError(MidiErrorType::event_type);
                                    return midiError.type();
                                }
                            }
                            break;
                        }
                        default: {
                            midiError(MidiErrorType::event_type);
                            return midiError.type();
                        }
                    }
                    last_eventType=now_eventType;
                    return MidiErrorType::noError;
                }
                MidiErrorType _read_fin(FILE*fin) {
                    if(!fin) {
                        midiError(MidiErrorType::filename);
                        #ifdef MIDI_DEBUG
                        midiDebug.write();
                        #endif
                        return MidiErrorType::filename;
                    }
                    //head
                    read_midiHead(fin);
                    if(midiError.type()!=MidiErrorType::noError){
                        #ifdef MIDI_DEBUG
                        midiDebug.write();
                        #endif
                        return midiError.type();
                    }
                    //track
                    read_midiTrack(fin);
                    if(midiError.type()!=MidiErrorType::noError){
                        #ifdef MIDI_DEBUG
                        midiDebug.write();
                        #endif
                        return midiError.type();
                    }
                    return MidiErrorType::noError;
                }
            public:
                MidiFile(void)=delete;
                MidiFile(std::string _filename):filename(_filename),head(),tracks(),m_state(MidiFileState::untouched) 
                {}
                MidiFile(const MidiFile&_midiFile):filename(_midiFile.filename),head(_midiFile.head),m_state(_midiFile.m_state){
                    if(_midiFile.tracks.size()){
                        tracks.resize(_midiFile.head.ntracks);
                        for(size_t i=0;i<_midiFile.head.ntracks;++i){
                            tracks[i]=_midiFile.tracks[i];
                        }
                    }
                    else{
                        
                    }
                }
                MidiFile(MidiFile&&_midiFile):filename(std::move(_midiFile.filename)),head(std::move(_midiFile.head)),tracks(std::move(_midiFile.tracks)),m_state(_midiFile.m_state){

                }
                ~MidiFile(void){
                    
                }
            public:
                MidiErrorType read(void) {
                    FILE*fin=fopen(filename.c_str(),"rb");
                    MidiErrorType ret=_read_fin(fin);
                    fclose(fin);
                    if(ret!=MidiErrorType::noError){
                        m_state=MidiFileState::read_error;
                        head.format=0;
                        head.ntracks=0;
                        head.division=0;
                        if(tracks.size()){
                            tracks.clear();
                        }
                    }
                    else{
                        m_state=MidiFileState::read_success;
                    }
                    return ret;
                }
            public:
                MidiFile&operator=(const MidiFile&_midi){
                    if(this!=&_midi){
                        this->~MidiFile();
                        filename=_midi.filename;
                        head=_midi.head;
                        tracks=_midi.tracks;
                    }
                    return *this;
                }
                MidiFile&operator=(MidiFile&&_midi){
                    this->~MidiFile();
                    filename=std::move(_midi.filename);
                    head=_midi.head;
                    tracks=_midi.tracks;
                    return *this;
                }
                MidiTrack&operator[](size_t idx) {
                    return tracks[idx];
                }
                const MidiTrack&operator[](size_t idx) const{
                    return tracks[idx];
                }
            public:
                bool is_untouched(void)const {
                    return m_state==MidiFileState::untouched;
                }
                bool is_read_success(void)const {
                    return m_state==MidiFileState::read_success;
                }
                bool is_read_error(void)const {
                    return m_state==MidiFileState::read_error;
                }
                bool is_write_success(void)const {
                    return m_state==MidiFileState::write_success;
                }
                bool is_write_error(void)const {
                    return m_state==MidiFileState::write_error;
                }
        };
    }
}

#endif
