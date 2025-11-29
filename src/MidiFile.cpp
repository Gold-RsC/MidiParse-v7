#include"MidiFile.hpp"
namespace GoldType{
    namespace MidiParse{
        uint32_t MidiFile::read_staticData(FILE*fin,size_t num){
            uint32_t ret=0;
            for(size_t i=0;i<num;++i){
                ret=(ret<<8)+fgetc(fin);
            }
            return ret;
        }
        MidiFile::__ReadDynamicData_t MidiFile::read_dynamicData(FILE*fin){
            __ReadDynamicData_t ret;
            ret.type=MidiErrorType::no_error;
            ret.data=0;
            ret.is_error=0;
            for(ret.length=0;ret.length<4;++ret.length){
                ret.buffer[ret.length]=fgetc(fin);
                ret.data=(ret.data<<7)|(ret.buffer[ret.length]&0x7F);
                if(ret.buffer[ret.length]<0x80){
                    ++ret.length;
                    return ret;
                }
            }
            ret.is_error=1;
            return ret;
        }
                
        MidiErrorType MidiFile::read_midiHead(FILE*fin){
            if(!(fgetc(fin)=='M'&&fgetc(fin)=='T'&&fgetc(fin)=='h'&&fgetc(fin)=='d')){
                return MidiErrorType::head_identification;
            }
            if(read_staticData(fin,4)!=6){
                return MidiErrorType::head_length;
            }
            head.format=read_staticData(fin,2);
            head.ntracks=read_staticData(fin,2);
            head.division=read_staticData(fin,2);
#ifdef MIDI_DEBUG
            return midiError(head);
#else
            return MidiErrorType::no_error;
#endif
        }
        MidiErrorType MidiFile::read_midiTrack(FILE*fin){
            tracks.resize(head.ntracks);
            size_t event_count=0;
            for(size_t trackIdx=0; trackIdx<head.ntracks; ++trackIdx) {
                if(fgetc(fin)!='M'||fgetc(fin)!='T'||fgetc(fin)!='r'||fgetc(fin)!='k') {
                    return MidiErrorType::track_identification;
                }
                uint32_t byte_num=read_staticData(fin,4);
                uint32_t byte_read=0;
                uint8_t last_eventType=(uint8_t)MidiEventType::null;
                while(!feof(fin)) {
                    MidiEvent event;
                    auto dynamicData=read_dynamicData(fin);
                    event.time=dynamicData.data;
                    event.timeMode=MidiTimeMode::tick;
                    byte_read+=dynamicData.length;
#ifdef MIDI_WARNING
                    if(dynamicData.is_error){
                        return midiError(MidiErrorType::event_deltaTime);
                    }
#endif
                    MidiErrorType err=read_midiMessage(fin,last_eventType,event,byte_read);
                    if(err!=MidiErrorType::no_error) {
                        return err;
                    }
                    tracks[trackIdx].emplace_back(event);
                    ++event_count;
                    if(event.is_meta()&&event[1]==0x2F) {
                        break;
                    }
                }
#ifdef MIDI_WARNING
                if(byte_num!=byte_read){
                    return MidiErrorType::track_length;
                }
#endif
            }
#ifdef MIDI_DEBUG
            return midiError(tracks);
#else
            return MidiErrorType::no_error;
#endif
        }
        MidiErrorType MidiFile::read_midiMessage(FILE*fin,uint8_t&last_eventType,MidiEvent&event,uint32_t&byte_read) {
            uint8_t now_eventType=fgetc(fin);
            ++byte_read;
            if(now_eventType<0x80) {
                if(last_eventType==(uint8_t)MidiEventType::null||(last_eventType&0xF0)==0xF0) {
                    return MidiErrorType::event_unknown_type;
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
                            auto dynamicData=read_dynamicData(fin);
                            
                            byte_read+=dynamicData.length+dynamicData.data;
                            if(dynamicData.is_error){
                                return MidiErrorType::meta_length;
                            }
                            for(size_t i=0; i<dynamicData.length;++i){
                                event.message.push_back(dynamicData.buffer[i]);
                            }
                            for(size_t i=0; i<dynamicData.data;++i){
                                event.message.push_back(fgetc(fin));
                            }
                            break;
                        }
                        case 0xF0:        //sysex_begin
                        case 0xF7: {      //sysex_end
                            event.message.push_back(now_eventType);
                            auto dynamicData=read_dynamicData(fin);
                            byte_read+=dynamicData.length+dynamicData.data;
                            if(dynamicData.is_error){
                                return MidiErrorType::sysex_length;
                            }
                            for(size_t i=0; i<dynamicData.length;++i){
                                event.message.push_back(dynamicData.buffer[i]);
                            }
                            for(size_t i=0; i<dynamicData.data;++i){
                                event.message.push_back(fgetc(fin));
                            }
                            break;
                        }
                        default: {
                            return MidiErrorType::event_unknown_type;
                        }
                    }
                    break;
                }
                default: {
                    return MidiErrorType::event_unknown_type;
                }
            }
            last_eventType=now_eventType;
            return MidiErrorType::no_error;
        }
        MidiErrorType MidiFile::_read_fin(FILE*fin) {
            if(!fin) {
                return midiError(MidiErrorType::filename);
            }
            MidiErrorType ret=MidiErrorType::no_error;
            //head
            ret=read_midiHead(fin);
            if(ret!=MidiErrorType::no_error){
                return midiError(ret);
            }
            //track
            ret=read_midiTrack(fin);
            if(ret!=MidiErrorType::no_error){
                return midiError(ret);
            }
            return ret;
        }
        MidiFile::MidiFile(std::string _filename):filename(_filename),head(),tracks(),m_state(MidiFileState::untouched) 
        {}
        MidiFile::MidiFile(const MidiFile&_midiFile):filename(_midiFile.filename),head(_midiFile.head),m_state(_midiFile.m_state){
            if(_midiFile.tracks.size()){
                tracks.resize(_midiFile.head.ntracks);
                for(size_t i=0;i<_midiFile.head.ntracks;++i){
                    tracks[i]=_midiFile.tracks[i];
                }
            }
            else{
                
            }
        }
        MidiFile::MidiFile(MidiFile&&_midiFile):filename(std::move(_midiFile.filename)),head(std::move(_midiFile.head)),tracks(std::move(_midiFile.tracks)),m_state(_midiFile.m_state){

        }
        MidiErrorType MidiFile::read(void) {
            FILE*fin=fopen(filename.c_str(),"rb");
            MidiErrorType ret=_read_fin(fin);
            fclose(fin);
            if(ret!=MidiErrorType::no_error){
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
        MidiFile&MidiFile::operator=(const MidiFile&_midi){
            if(this!=&_midi){
                this->~MidiFile();
                filename=_midi.filename;
                head=_midi.head;
                tracks=_midi.tracks;
            }
            return *this;
        }
        MidiFile&MidiFile::operator=(MidiFile&&_midi){
            this->~MidiFile();
            filename=std::move(_midi.filename);
            head=_midi.head;
            tracks=_midi.tracks;
            return *this;
        }
        MidiTrack&MidiFile::operator[](size_t idx) {
            return tracks[idx];
        }
        const MidiTrack&MidiFile::operator[](size_t idx) const{
            return tracks[idx];
        }
        bool MidiFile::is_untouched(void)const {
            return m_state==MidiFileState::untouched;
        }
        bool MidiFile::is_read_success(void)const {
            return m_state==MidiFileState::read_success;
        }
        bool MidiFile::is_read_error(void)const {
            return m_state==MidiFileState::read_error;
        }
        bool MidiFile::is_write_success(void)const {
            return m_state==MidiFileState::write_success;
        }
        bool MidiFile::is_write_error(void)const {
            return m_state==MidiFileState::write_error;
        }

        // MidiErrorType get_error(MidiError&_midiError,const MidiFile&file){
        //     MidiErrorType type=MidiErrorType::no_error;
        //     type=get_error(_midiError,file.head);
        //     if(type!=MidiErrorType::no_error){
        //         return type;
        //     }
        //     type=get_error(_midiError,file.tracks);
        //     return type;
        // }
    }
}
