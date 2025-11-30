/********************************************************************************************************
 * File Name    : MidiParser.hpp
 * Author       : Csrua / Gold_RsC
 * github       : Gold-RsC(https://github.com/Gold-RsC)
 * bilibili     : Csrua(https://space.bilibili.com/361846321)
 * QQ           : 310106329
 * Email        : 310106329@qq.com
 * Create Date  : 2020/07/26
 * Update Date  : 2025/12/01
 * Details      : README.md 
 ********************************************************************************************************/
#ifndef MIDIPARSER_HPP
#define MIDIPARSER_HPP
#include"MidiFile.hpp"
#include"MoreMidiEvent.hpp"
namespace GoldType{
    namespace MidiParse{
        struct __get_tempo_time_fun{
            uint64_t operator()(const Tempo&tempo)const{
                return tempo.time;
            }
        };
        struct __get_tempo_timeNode_fun{
            uint64_t operator()(const Tempo&tempo)const{
                return tempo.timeNode;
            }
        };
        template<typename _MidiEvent>
        struct __tick2micro_fun{
            uint16_t _tpqn;
            __tick2micro_fun(uint16_t _tpqn):
                _tpqn(_tpqn) {}
            void operator()(_MidiEvent&_event,const Tempo&_tempo)const{
                _event.timeMode=MidiTimeMode::microsecond;
                _event.time=_tempo.timeNode
                        +(_event.time-_tempo.time)
                        *_tempo.mispqn/_tpqn;
            }
        };
        template<typename _MidiEvent>
        struct __micro2tick_fun{
            uint16_t _tpqn;
            __micro2tick_fun(uint16_t _tpqn):
                _tpqn(_tpqn) {}
            void operator()(_MidiEvent&_event,const Tempo&_tempo)const{
                _event.timeMode=MidiTimeMode::tick;
                _event.time=_tempo.timeNode
                        +(_event.time-_tempo.time)
                        *_tpqn/_tempo.mispqn;
            }
        };
        class MidiParser{
            private:
                MidiTimeMode m_timeMode;
                MidiHead m_head;
                TempoMap m_tempoMap;
                BarBeatMap m_bbMap;
                NoteMap m_noteMap;
                TextMap m_textMap;
            protected:
                uint8_t get_metaTrack(uint8_t _track)const;

				MidiErrorType parse_tick(const MidiFile&m_midi);
                MidiErrorType parse_micro(const MidiFile&m_midi);

                template<typename _MidiEvent>
                bool is_time_sorted(const MidiEventMap<_MidiEvent>&_map)const {
                    for(size_t trackIdx=0;trackIdx<_map.size();++trackIdx){
                        for(size_t eventIdx=1;eventIdx<_map[trackIdx].size();++eventIdx){
                            if(_map[trackIdx][eventIdx].time<_map[trackIdx][eventIdx-1].time){
                                return false;
                            }
                        }
                    }
                    return true;
                }
                
                size_t _find_bbIdx_between(uint64_t _time,const BarBeatList&_bbList,size_t _left,size_t _right)const {
                    if(_left>=_right){
                        return size_t(-1);
                    }
                    size_t mid=(_left+_right)/2;

                    if(_bbList[mid].time<=_time){
                        if(mid!=_bbList.size()-1){
                            if(_bbList[mid+1].time>_time){
                                return mid;
                            }
                            else{
                                return _find_bbIdx_between(_time,_bbList,mid+1,_right);
                            }
                        }
                        else{
                            return mid;
                        }
                    }
                    return _find_bbIdx_between(_time,_bbList,_left,mid);
                }
                template<typename _MidiEvent>
                MidiErrorType set_bb_sorted(MidiEventMap<_MidiEvent>&_map){
                    for(size_t trackIdx=0;trackIdx<_map.size();++trackIdx){
                        size_t bbIdx=0;
                        uint8_t metaTrack=get_metaTrack(trackIdx);
                        for(size_t eventIdx=0;eventIdx<_map[trackIdx].size();++eventIdx){
                            _MidiEvent&event=_map[trackIdx][eventIdx];
                            while(m_bbMap[metaTrack][bbIdx].time<event.time){
                                if(bbIdx<m_bbMap[metaTrack].size()-1){
                                    if(m_bbMap[metaTrack][bbIdx+1].time<=event.time){
                                        ++bbIdx;
                                        continue;
                                    }
                                }
                                break;
                            }
                            if(m_bbMap[metaTrack][bbIdx].time>event.time){
                                return MidiErrorType::change_timeMode;
                            }
                            double deltaBeat=(event.time-m_bbMap[metaTrack][bbIdx].time)
                                            *m_bbMap[metaTrack][bbIdx].denominator
                                            /4.0/m_head.tpqn();
                            double deltaBar=deltaBeat/m_bbMap[metaTrack][bbIdx].numerator;
                            event.bar=m_bbMap[metaTrack][bbIdx].barNode+deltaBar;
                            event.beat=m_bbMap[metaTrack][bbIdx].beatNode+deltaBeat;
                        }
                    }
                    return MidiErrorType::no_error;
                }
                
                template<typename _MidiEvent,typename _Fun>
                MidiErrorType set_bb_unsorted(MidiEventMap<_MidiEvent>&_map){
                    for(size_t trackIdx=0;trackIdx<_map.size();++trackIdx){
                        uint8_t metaTrack=get_metaTrack(trackIdx);
                        for(size_t eventIdx=0;eventIdx<_map[trackIdx].size();++eventIdx){
                            _MidiEvent&event=_map[trackIdx][eventIdx];
                            size_t bbIdx=_find_bbIdx_between(event.time,m_bbMap[metaTrack],0,m_bbMap[metaTrack].size());
                            if(bbIdx==size_t(-1)){
                                double deltaBeat=(event.time-m_bbMap[metaTrack][bbIdx].time)
                                                *m_bbMap[metaTrack][bbIdx].denominator
                                                /4.0/m_head.tpqn();
                                double deltaBar=deltaBeat/m_bbMap[metaTrack][bbIdx].numerator;
                                event.bar=m_bbMap[metaTrack][bbIdx].barNode+deltaBar;
                                event.beat=m_bbMap[metaTrack][bbIdx].beatNode+deltaBeat;
                            }
                            else{
                                return MidiErrorType::change_timeMode;
                            }
                        }
                    }
                    return MidiErrorType::no_error;
                }
                
                template<typename _GetTime>
                size_t _find_tempoIdx_between(uint64_t _time,const TempoList&_tempoList,size_t _left,size_t _right,_GetTime&&_getTime)const {
                    if(_left>=_right){
                        return size_t(-1);
                    }
                    size_t mid=(_left+_right)/2;

                    if(_getTime(_tempoList[mid])<=_time){
                        if(mid!=_tempoList.size()-1){
                            if(_getTime(_tempoList[mid+1])>_time){
                                return mid;
                            }
                            else{
                                return _find_tempoIdx_between(_time,_tempoList,mid+1,_right,_getTime);
                            }
                        }
                        else{
                            return mid;
                        }
                    }
                    return _find_tempoIdx_between(_time,_tempoList,_left,mid,_getTime);
                }
                
                template<typename _MidiEvent,typename _Fun,typename _GetTime>
                MidiErrorType change_timeMode_sorted(MidiEventMap<_MidiEvent>&_map,_Fun&&_fun,_GetTime&&_getTime)const {
                    for(size_t trackIdx=0;trackIdx<_map.size();++trackIdx){
                        size_t tempoIdx=0;
                        uint8_t metaTrack=get_metaTrack(trackIdx);
                        for(size_t eventIdx=0;eventIdx<_map[trackIdx].size();++eventIdx){
                            _MidiEvent&event=_map[trackIdx][eventIdx];
                            while(_getTime(m_tempoMap[metaTrack][tempoIdx])<event.time){
                                if(tempoIdx<m_tempoMap[metaTrack].size()-1){
                                    if(_getTime(m_tempoMap[metaTrack][tempoIdx+1])<=event.time){
                                        ++tempoIdx;
                                        continue;
                                    }
                                }
                                break;
                            }
                            if(_getTime(m_tempoMap[metaTrack][tempoIdx])>event.time){
                                return MidiErrorType::change_timeMode;
                            }
                            _fun(event,m_tempoMap[metaTrack][tempoIdx]);
                        }
                    }
                    return MidiErrorType::no_error;
                }
                
                template<typename _MidiEvent,typename _Fun,typename _GetTime>
                MidiErrorType change_timeMode_unsorted(MidiEventMap<_MidiEvent>&_map,_Fun&&_fun,_GetTime&&_getTime)const {
                    for(size_t trackIdx=0;trackIdx<_map.size();++trackIdx){
                        uint8_t metaTrack=get_metaTrack(trackIdx);
                        for(size_t eventIdx=0;eventIdx<_map[trackIdx].size();++eventIdx){
                            _MidiEvent&event=_map[trackIdx][eventIdx];
                            size_t tempoIdx=_find_tempoIdx_between(event.time,m_tempoMap[metaTrack],0,m_tempoMap[metaTrack].size(),_getTime);
                            if(tempoIdx==size_t(-1)){
                                _fun(event,m_tempoMap[metaTrack][tempoIdx]);
                            }
                            else{
                                return MidiErrorType::change_timeMode;
                            }
                        }
                    }
                    return MidiErrorType::no_error;
                }
                
                template<typename _MidiEvent>
                MidiErrorType change_timeMode_tick2micro_sorted(MidiEventMap<_MidiEvent>&_map)const {
                    if(m_timeMode==MidiTimeMode::tick){
                        return change_timeMode_sorted(
                            _map,
                            __tick2micro_fun<_MidiEvent>(m_head.tpqn()),
                            __get_tempo_time_fun()
                        );
                    }
                    return change_timeMode_sorted(
                        _map,
                        __tick2micro_fun<_MidiEvent>(m_head.tpqn()),
                        __get_tempo_timeNode_fun()
                    );
                }
                
                template<typename _MidiEvent>
                MidiErrorType change_timeMode_micro2tick_sorted(MidiEventMap<_MidiEvent>&_map)const {
                    if(m_timeMode==MidiTimeMode::microsecond){
                        return change_timeMode_sorted(
                            _map,
                            __micro2tick_fun<_MidiEvent>(m_head.tpqn()),
                            __get_tempo_time_fun()
                        );
                    }
                    return change_timeMode_sorted(
                        _map,
                        __micro2tick_fun<_MidiEvent>(m_head.tpqn()),
                        __get_tempo_timeNode_fun()
                    );
                }
                
                template<typename _MidiEvent>
                MidiErrorType change_timeMode_tick2micro_unsorted(MidiEventMap<_MidiEvent>&_map)const {
                    if(m_timeMode==MidiTimeMode::tick){
                        return change_timeMode_unsorted(
                            _map,
                            __tick2micro_fun<_MidiEvent>(m_head.tpqn()),
                            __get_tempo_time_fun()
                        );
                    }
                    return change_timeMode_unsorted(
                        _map,
                        __tick2micro_fun<_MidiEvent>(m_head.tpqn()),
                        __get_tempo_timeNode_fun()
                    );
                }
                
                template<typename _MidiEvent>
                MidiErrorType change_timeMode_micro2tick_unsorted(MidiEventMap<_MidiEvent>&_map)const {
                    if(m_timeMode==MidiTimeMode::microsecond){
                        return change_timeMode_unsorted(
                            _map,
                            __micro2tick_fun<_MidiEvent>(m_head.tpqn()),
                            __get_tempo_time_fun()
                        );
                    }
                    return change_timeMode_unsorted(
                        _map,
                        __micro2tick_fun<_MidiEvent>(m_head.tpqn()),
                        __get_tempo_timeNode_fun()
                    );
                }
            
            public:
                MidiParser(void)=delete;
                MidiParser(const MidiParser&_midiParser);
                MidiParser(MidiParser&&_midiParser);
                MidiParser(const MidiFile&_midi,MidiTimeMode _timeMode=MidiTimeMode::tick);
                MidiParser(MidiFile&&_midi,MidiTimeMode _timeMode=MidiTimeMode::tick);
                MidiParser(std::string _filename,MidiTimeMode _timeMode=MidiTimeMode::tick);
                ~MidiParser(void)=default;
            public:
                template<typename _MidiEvent>
                MidiErrorType change_timeMode(MidiEventMap<_MidiEvent>&_map,MidiTimeMode _mode=MidiTimeMode::microsecond)const {
                    MidiTimeMode _oldTimeMode=_map.get_timeMode();
                    MidiErrorType err=MidiErrorType::no_error;
                    if(_oldTimeMode==_mode){
                        return MidiErrorType::no_error;
                    }
                    if(_oldTimeMode==MidiTimeMode::tick&&_mode==MidiTimeMode::microsecond){
                        if(is_time_sorted(_map)){
                            err=change_timeMode_tick2micro_sorted(_map);
                        }
                        else{
                            err=change_timeMode_tick2micro_unsorted(_map);
                        }
                    }
                    else if(_oldTimeMode==MidiTimeMode::microsecond&&_mode==MidiTimeMode::tick){
                        if(is_time_sorted(_map)){
                            err=change_timeMode_micro2tick_sorted(_map);
                        }
                        else{
                            err=change_timeMode_micro2tick_unsorted(_map);
                        }
                    }
                    return err;
                }
                MidiErrorType change_timeMode(MidiTimeMode _mode=MidiTimeMode::microsecond);
            public:
                uint16_t format(void)const;
                uint16_t ntracks(void)const;
                uint16_t division(void)const;
                uint16_t tpqn(void)const;
                const NoteMap&noteMap(void)const;
                const BarBeatMap&bbMap(void)const;
                const TempoMap&tempoMap(void)const;
                const TextMap&textMap(void)const;

                MidiTimeMode timeMode(void)const;
            public:
                MidiParser& operator=(const MidiParser&)=default;
                MidiParser& operator=(MidiParser&&)=default;
        };
    }
}
#endif