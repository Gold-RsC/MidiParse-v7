#ifndef MIDIEVENT_HPP
#define MIDIEVENT_HPP
#include"MidiMessage.hpp"
#include<algorithm>
namespace GoldType{
    namespace MidiParse{
        /********************************
                    Midi Event
        ********************************/
        
        
        enum class MidiTimeMode:bool{
            tick=0,
            microsecond=1
        };
        
        class BasicMidiEvent{
            public:
                static uint64_t __time_error_v;
                uint64_t time;
                MidiTimeMode timeMode:1;
                uint8_t track:7;
            public:
                BasicMidiEvent(uint64_t _time=0,MidiTimeMode _timeMode=MidiTimeMode::tick,uint8_t _track=0):
                    time(_time),timeMode(_timeMode),track(_track){}
                BasicMidiEvent(const BasicMidiEvent&)=default;
                virtual ~BasicMidiEvent(void)=default;
            public:
                virtual uint64_t&tick(void){
                    if(timeMode==MidiTimeMode::tick){
                        return time;
                    }
                    return __time_error_v;
                }
                virtual const uint64_t&tick(void)const {
                    if(timeMode==MidiTimeMode::tick){
                        return time;
                    }
                    return __time_error_v;
                }
                virtual uint64_t&microsecond(void){
                    if(timeMode==MidiTimeMode::microsecond){
                        return time;
                    }
                    return __time_error_v;
                }
                virtual const uint64_t&microsecond(void)const {
                    if(timeMode==MidiTimeMode::microsecond){
                        return time;
                    }
                    return __time_error_v;
                }
        };
        uint64_t BasicMidiEvent::__time_error_v=uint64_t(-1);
        
        class BasicMidiEvent_Non:public BasicMidiEvent{
            public:
                uint8_t channel:4;
            public:
                BasicMidiEvent_Non(uint64_t _time=0,MidiTimeMode _timeMode=MidiTimeMode::tick,uint8_t _track=0,uint8_t _channel=0):
                    BasicMidiEvent(_time,_timeMode,_track),channel(_channel){}
                BasicMidiEvent_Non(const BasicMidiEvent_Non&)=default;
                virtual ~BasicMidiEvent_Non(void)=default;
        };
        class BasicMidiEvent_Meta:public BasicMidiEvent{
            public:
                BasicMidiEvent_Meta(uint64_t _time=0,MidiTimeMode _timeMode=MidiTimeMode::tick,uint8_t _track=0):
                    BasicMidiEvent(_time,_timeMode,_track){}
                BasicMidiEvent_Meta(const BasicMidiEvent_Meta&)=default;
                virtual ~BasicMidiEvent_Meta(void)=default;
        };
        
        class MidiEvent:public BasicMidiEvent {
            public:
                MidiMessage message;
            public:
                MidiEvent(void):
                    message() {}
                MidiEvent(const MidiEvent&another)=default;
                MidiEvent(const MidiMessage&_message):
                    message(_message) {}
                ~MidiEvent(void){}
            public:
                MidiEventType type(void)const {
                    return message.type();
                }
                uint8_t channel(void)const {
                    return message.channel();
                }
            public:
                bool is_normal(void)const{
                    return !(is_meta()||is_sysex());
                }
                bool is_meta(void)const {
                    return type()==MidiEventType::meta;
                }
                bool is_sysex(void)const{
                    return type()==MidiEventType::sysex_begin||type()==MidiEventType::sysex_end;
                }
            public:
                uint8_t&operator[](size_t idx) {
                    return message[idx];
                }
                const uint8_t&operator[](size_t idx) const{
                    return message[idx];
                }
                MidiEvent&operator=(const MidiEvent&another)=default;
        };
        

        template<typename _MidiEvent>
        class MidiEventList:public std::vector<_MidiEvent>{
            public:
                using std::vector<_MidiEvent>::vector;
            public:
                MidiTimeMode get_timeMode(void)const {
                    if(this->size()){
                        MidiTimeMode ret=this->front().timeMode;
                        for(size_t i=1;i<this->size();++i){
                            if(ret!=this->operator[](i).timeMode){
                                return MidiTimeMode::tick;
                            }
                        }
                        return ret;
                    }
                    return MidiTimeMode::tick;
                }
                void sort(void){
                    std::sort(this->begin(),this->end());
                }
        };

        template<typename _MidiEvent>
        class MidiEventMap:public std::vector<MidiEventList<_MidiEvent>>{
            public:
                using std::vector<MidiEventList<_MidiEvent>>::vector;
            public:
                MidiTimeMode get_timeMode(void)const {
                    if(this->size()){
                        MidiTimeMode ret=this->front().get_timeMode();
                        for(size_t i=1;i<this->size();++i){
                            if(ret!=this->operator[](i).get_timeMode()){
                                return MidiTimeMode::tick;
                            }
                        }
                        return ret;
                    }
                    return MidiTimeMode::tick;
                }
                void sort(void){
                    for(size_t i=0;i<this->size();++i){
                        this->operator[](i).sort();
                    }
                }
        };
        template<typename _MidiEvent>
        MidiEventList<_MidiEvent> event_map_to_list(const MidiEventMap<_MidiEvent>&_map){
            size_t _size=0;
            for(size_t i=0;i<_map.size();++i){
                _size+=_map[i].size();
            }
            MidiEventList<_MidiEvent> ret;
            ret.reserve(_size);
            for(uint8_t trackIdx=0;trackIdx<_map.size();++trackIdx){
                ret.insert(ret.end(),_map[trackIdx].begin(),_map[trackIdx].end());
            }
            return ret;
        }
        template<typename _MidiEvent>
        MidiEventMap<_MidiEvent> event_list_to_map(const MidiEventList<_MidiEvent>&_list){
            size_t _size[128]={0};
            uint8_t _max_track=0;
            for(size_t i=0;i<_list.size();++i){
                ++_size[_list[i].track];
                _max_track=std::max(_list[i].track,_max_track);
            }
            MidiEventMap<_MidiEvent> ret(_max_track+1);
            for(size_t i=0;i<=_max_track;++i){
                ret[i].reserve(_size[i]);
            }
            for(size_t i=0;i<_list.size();++i){
                ret[_list[i].track].emplace_back(_list[i]);
            }
            return ret;
        }
        
        template<typename _MidiEvent>
        uint8_t get_ntracks(const MidiEventList<_MidiEvent>&_list){
            uint8_t _num=0;
            for(typename MidiEventList<_MidiEvent>::iterator it=_list.begin();it!=_list.end();++it){
                _num=max(_num,it->track);
            }
            return _num+1;
        }
        template<typename _MidiEvent>
        uint8_t get_ntracks(const MidiEventMap<_MidiEvent>&_map){
            return _map.size();
        }
        
        template<typename _MidiEvent,typename _Fun,typename..._Args>
        void for_event(MidiEventList<_MidiEvent>&_list,_Fun&&_fun,_Args&&..._args){
            for(size_t eventIdx=0;eventIdx<_list.size();++eventIdx){
                _fun(_list[eventIdx],std::forward(_args)...);
            }
        }
        template<typename _MidiEvent,typename _Fun,typename..._Args>
        void for_event(MidiEventMap<_MidiEvent>&_map,_Fun&&_fun,_Args&&..._args){
            for(uint8_t trackIdx=0;trackIdx<_map.size();++trackIdx){
                for(size_t eventIdx=0;eventIdx<_map[trackIdx].size();++eventIdx){
                    _fun(_map[trackIdx][eventIdx],std::forward(_args)...);
                }
            }
        }

        template<>
        MidiErrorType MidiError::operator()<MidiEvent>(const MidiEvent&_midiEvent){
            MidiErrorType type=MidiErrorType::no_error;
            switch(_midiEvent.type()){
                case MidiEventType::note_off:
                case MidiEventType::note_on:
                case MidiEventType::key_afterTouch:
                case MidiEventType::controller:
                case MidiEventType::pitchWheel:{
                    if(_midiEvent[1]&0x80){
                        return MidiErrorType((uint8_t)type|0x01);
                    }
                    if(_midiEvent[2]&0x80){
                        return MidiErrorType((uint8_t)type|0x02);
                    }
                    break;
                }
                case MidiEventType::program:
                case MidiEventType::channel_afterTouch:{
                    if(_midiEvent[1]&0x80){
                        return MidiErrorType((uint8_t)type|0x01);
                    }
                    break;
                }
                case MidiEventType::sysex_begin:
                case MidiEventType::sysex_end:{
                    size_t i=1;
                    uint32_t length=0;
                    for(;i<5;++i) {
                        length=(length<<7)|(_midiEvent[i]&0x7F);
                        if(_midiEvent[i]<0x80) {
                            break;
                        }
                    }
                    if(i+length+1!=_midiEvent.message.size()){
                        return MidiErrorType::sysex_length;
                    }
                    break;
                }
                case MidiEventType::meta:{
                    size_t i=2;
                    uint32_t length=0;
                    for(;i<5;++i) {
                        length=(length<<7)|(_midiEvent[i]&0x7F);
                        if(_midiEvent[i]<0x80) {
                            break;
                        }
                    }
                    if(i+length+1!=_midiEvent.message.size()){
                        return MidiErrorType::meta_length;
                    }
                    break;
                }
                default:{
                    return MidiErrorType::event_unknown_type;
                }
            }
            return MidiErrorType::no_error;
        }
    }
}
#endif
