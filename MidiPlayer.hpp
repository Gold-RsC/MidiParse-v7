#ifndef MIDIPLAYER_HPP
#define MIDIPLAYER_HPP
#include<thread>
#include<mutex>
#include<windows.h>
#include<algorithm>
#include<condition_variable>
#pragma comment(lib,"winmm.lib")
#include"MidiParser.hpp"
namespace GoldType{
    namespace MidiParse{
        class MidiShortMessage{
            public:
                uint64_t time;
                uint32_t message;
            public:
                MidiShortMessage(uint64_t time=0,uint32_t message=0):time(time),message(message){
                }
                
                MidiShortMessage(uint64_t _time,const MidiMessage&_message){
                    time=_time;
                    switch(_message.type()){
                        case MidiEventType::note_off:
                        case MidiEventType::note_on:
                        case MidiEventType::key_afterTouch:
                        case MidiEventType::controller:
                        case MidiEventType::pitchWheel:
                        {
                            message=_message[0]|_message[1]<<8|_message[2]<<16;
                            break;
                        }
                        case MidiEventType::program:
                        case MidiEventType::channel_afterTouch:{
                            message=_message[0]|_message[1]<<8;
                            break;
                        }
                        default:{
                            
                        }
                    }
                }
                MidiShortMessage(const MidiEvent&_event):MidiShortMessage(_event.time,_event.message){
                }
        };
        bool operator==(const MidiShortMessage&a,const MidiShortMessage&b){
            return a.time==b.time&&a.message==b.message;
        }
        bool operator!=(const MidiShortMessage&a,const MidiShortMessage&b){
            return !(a==b);
        }
        bool operator<(const MidiShortMessage&a,const MidiShortMessage&b){
            if(a.time!=b.time){
                return a.time<b.time;
            }
            return a.message<b.message;
        }
        bool operator>(const MidiShortMessage&a,const MidiShortMessage&b){
            if(a.time!=b.time){
                return a.time>b.time;
            }
            return a.message>b.message;
        }
        bool operator<=(const MidiShortMessage&a,const MidiShortMessage&b){
            return !(a>b);
        }
        bool operator>=(const MidiShortMessage&a,const MidiShortMessage&b){
            return !(a<b);
        }
        class MidiShortMessageList:public std::vector<MidiShortMessage>{
            private:
            public:
                using std::vector<MidiShortMessage>::vector;
                MidiShortMessageList(const MidiFile&_file){
                    if(_file.is_read_success()){
                        MidiParser _parser(_file);
                        MidiTrackList tracks=_file.tracks;
                        time_delta2absolute(tracks);
                        _parser.change_timeMode(tracks,MidiTimeMode::microsecond);
                        for_event(tracks,[this](const MidiEvent&event){
                            if(event.is_normal()){
                                emplace_back(event);
                            }
                        });
                        std::sort(this->begin(),this->end());
                    }
                }
                MidiShortMessageList(const MidiParser&_parser){
                    NoteMap noteMap=_parser.noteMap();
                    if(_parser.timeMode()==MidiTimeMode::microsecond){
                        *this=MidiShortMessageList(noteMap);
                    }
                    else{
                        _parser.change_timeMode(noteMap,MidiTimeMode::microsecond);
                        *this=MidiShortMessageList(noteMap);
                    }
                }
                MidiShortMessageList(const std::string&_filename){
                    MidiFile file(_filename);
                    file.read();
                    *this=MidiShortMessageList(file);
                }
                MidiShortMessageList(NoteList _noteList){
                    _noteList.sort();
                    this->reserve(_noteList.size()+_noteList.size()/10);
                    uint8_t instruments[16]={0};
                    for(const Note&note:_noteList){
                        if(note.timeMode==MidiTimeMode::tick){
                            continue;
                        }
                        if(note.instrument!=instruments[note.channel]){
                            instruments[note.channel]=note.instrument;
                            emplace_back(note.time,(note.instrument<<8)|0xC0|note.channel);
                        }
                        if(note.velocity){
                            emplace_back(note.time,(note.velocity<<16)|(note.pitch<<8)|0x90|note.channel);
                        }
                        else{
                            emplace_back(note.time,(note.pitch<<8)|0x80|note.channel);
                        }
                    }
                    std::sort(this->begin(),this->end());
                }
                MidiShortMessageList(const NoteMap&_map):MidiShortMessageList(event_map_to_list(_map)){
                }
            public:
                MidiTimeMode get_timeMode(void)const {
                    return MidiTimeMode::microsecond;
                }
        };
        class MidiPlayer{
            public:
                enum class State{
                    stopped,
                    playing,
                    paused,
                };
            private:
                MidiShortMessageList m_messages;

                std::thread m_thread;
            
                State m_state;
                std::mutex m_mutex;
                std::condition_variable m_condition;
                

                uint64_t m_time;
            private:
                void play_helper(void){
                    HMIDIOUT handle;
                    midiOutOpen(&handle,0,0,0,CALLBACK_NULL);
                    
                    for(MidiShortMessageList::iterator it=m_messages.begin();it!=m_messages.end();++it){
                        uint64_t wait_time;
                        {
                            std::unique_lock<std::mutex> lock(m_mutex);
                            if(m_state==State::paused){
                                midiOutReset(handle);
                                m_condition.wait(lock,[&]{
                                    return m_state!=State::paused;
                                });
                                if(m_state==State::stopped){
                                    m_time=0;
                                    midiOutClose(handle);
                                    return;
                                }
                            }
                            else if(m_state==State::stopped){
                                m_time=0;
                                midiOutClose(handle);
                                return;
                            }
                            wait_time=it->time-m_time;
                            m_time=it->time;
                        }
                        if(wait_time/1000){
                            std::this_thread::sleep_for(std::chrono::milliseconds(wait_time/1000));
                        }
                        midiOutShortMsg(handle,it->message);
                    }

                    
                    std::this_thread::sleep_for(std::chrono::seconds(5));
                    midiOutClose(handle);
                    m_state=State::stopped;
                }
            public:
                MidiPlayer(void):
                    m_state(State::stopped),
                    m_time(0){

                }
                MidiPlayer(const MidiShortMessageList&_messages):
                    m_messages(_messages),
                    m_state(State::stopped),
                    m_time(0){

                }
                MidiPlayer(const std::string&_filename):
                    m_messages(_filename),
                    m_state(State::stopped),
                    m_time(0){

                }
                MidiPlayer(const NoteList&_noteList):
                    m_messages(_noteList),
                    m_state(State::stopped),
                    m_time(0){
                        
                }
                MidiPlayer(const NoteMap&_noteMap):
                    m_messages(_noteMap),
                    m_state(State::stopped),
                    m_time(0){
                        
                    
                }
                ~MidiPlayer(void){
                    join();
                }
            public:
                void play(void){
                    std::lock_guard<std::mutex> lock(m_mutex);
                    if(m_state==State::paused){
                        m_state=State::playing;
                        m_condition.notify_one();
                    }
                    else{
                        m_state=State::playing;
                        m_thread=std::thread(&MidiPlayer::play_helper,this);
                    }

                }
                void pause(void){
                    std::lock_guard<std::mutex> lock(m_mutex);
                    
                    m_state=State::paused;
                }
                void stop(void){
                    std::lock_guard<std::mutex> lock(m_mutex);
                    if(m_state==State::playing){
                    }
                    else if(m_state==State::paused){

                    }
                    m_state=State::stopped;
                }
                void join(void){
                    if(m_thread.joinable()){
                        m_thread.join();
                    }
                }
                State state(void){
                    std::lock_guard<std::mutex> lock(m_mutex);
                    return m_state;
                }
                uint64_t time(void){
                    std::lock_guard<std::mutex> lock(m_mutex);
                    return m_time;
                }
        };
    }
}            
#endif
