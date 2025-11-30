#ifndef BASICMIDIEVENT_HPP
#define BASICMIDIEVENT_HPP
#include"MidiError.hpp"
#include<vector>
#include<algorithm>
namespace GoldType{
    namespace MidiParse{
        enum MidiTimeMode:bool{
            tick=0,
            microsecond=1
        };
        
        class BasicMidiEvent:public MidiObject{
            public:
                static uint64_t __time_error_v;
                uint64_t time;
                MidiTimeMode timeMode:1;
                uint8_t track:7;
            public:
                BasicMidiEvent(uint64_t _time=0,MidiTimeMode _timeMode=MidiTimeMode::tick,uint8_t _track=0);
                BasicMidiEvent(const BasicMidiEvent&)=default;
                virtual ~BasicMidiEvent(void)=default;
            public:
                virtual uint64_t&tick(void);
                virtual const uint64_t&tick(void)const;
                virtual uint64_t&microsecond(void);
                virtual const uint64_t&microsecond(void)const;

                MidiErrorType get_error(MidiError&_midiError=midiError)const override;
        };
        
        class BasicMidiEvent_Non:public BasicMidiEvent{
            public:
                uint8_t channel:4;
            public:
                BasicMidiEvent_Non(uint64_t _time=0,MidiTimeMode _timeMode=MidiTimeMode::tick,uint8_t _track=0,uint8_t _channel=0);
                BasicMidiEvent_Non(const BasicMidiEvent_Non&)=default;
                virtual ~BasicMidiEvent_Non(void)=default;
            public:
                MidiErrorType get_error(MidiError&_midiError)const override;
        };
        class BasicMidiEvent_Meta:public BasicMidiEvent{
            public:
                BasicMidiEvent_Meta(uint64_t _time=0,MidiTimeMode _timeMode=MidiTimeMode::tick,uint8_t _track=0);
                BasicMidiEvent_Meta(const BasicMidiEvent_Meta&)=default;
                virtual ~BasicMidiEvent_Meta(void)=default;
            public:
                MidiErrorType get_error(MidiError&_midiError=midiError)const override;
        };

        template<typename _MidiEvent>
        class MidiEventList:public std::vector<_MidiEvent>,public MidiObject{
            public:
                using std::vector<_MidiEvent>::vector;
            public:
                void to_abs(void){
                    uint64_t last_time=0;
                    for(size_t i=0;i<this->size();++i){
                        last_time+=this->operator[](i).time;
                        this->operator[](i).time=last_time;
                    }
                }
                void to_delta(void){
                    uint64_t last_time=0;
                    for(size_t i=0;i<this->size();++i){
                        this->operator[](i).time-=last_time;
                        last_time+=this->operator[](i).time;
                    }
                }
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

                MidiErrorType get_error(MidiError&_midiError=midiError)const override final{
                    MidiErrorType ret=MidiErrorType::no_error;
                    for(size_t i=0;i<this->size();++i){
                        ret=this->operator[](i).get_error(_midiError);
                        if(ret!=MidiErrorType::no_error){
                            return _midiError(ret);
                        }
                    }
                    return _midiError(ret);
                }
        };
        template<typename _MidiEvent>
        class MidiEventMap:public std::vector<MidiEventList<_MidiEvent>>,public MidiObject{
            public:
                using std::vector<MidiEventList<_MidiEvent>>::vector;
            public:
                void to_abs(void){
                    for(size_t i=0;i<this->size();++i){
                        this->operator[](i).to_abs();
                    }
                }
                void to_delta(void) {
                    for(size_t i=0;i<this->size();++i){
                        this->operator[](i).to_delta();
                    }
                }
                MidiTimeMode get_timeMode(void)const{
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
                MidiErrorType get_error(MidiError&_midiError=midiError)const override final{
                    MidiErrorType ret=MidiErrorType::no_error;
                    for(size_t i=0;i<this->size();++i){
                        ret=this->operator[](i).get_error(_midiError);
                        if(ret!=MidiErrorType::no_error){
                            return _midiError(ret);
                        }
                    }
                    return _midiError(ret);
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
        void for_event(const MidiEventList<_MidiEvent>&_list,_Fun&&_fun,_Args&&..._args){
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
        template<typename _MidiEvent,typename _Fun,typename..._Args>
        void for_event(const MidiEventMap<_MidiEvent>&_map,_Fun&&_fun,_Args&&..._args){
            for(uint8_t trackIdx=0;trackIdx<_map.size();++trackIdx){
                for(size_t eventIdx=0;eventIdx<_map[trackIdx].size();++eventIdx){
                    _fun(_map[trackIdx][eventIdx],std::forward(_args)...);
                }
            }
        }
    }
}
#endif