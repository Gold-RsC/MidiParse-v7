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
                uint8_t get_metaTrack(uint8_t _track)const{
                    if(m_head.format==0||m_head.format==0x01){
                        return 0;
                    }
                    else if(m_head.format==0x02){
                        return _track;
                    }
                    return 0xFF;
                }
                
				MidiErrorType parse_tick(const MidiFile&m_midi){
                    uint8_t instruments[16]={0};
                    for(size_t trackIdx=0;trackIdx<m_head.ntracks;++trackIdx){
                        uint64_t time=0;
                        uint8_t metaTrack=get_metaTrack(trackIdx);
                        if(metaTrack==0xFF){
                            return MidiErrorType::parse_error;
                        }

                        for(size_t eventIdx=0;eventIdx<m_midi[trackIdx].size();++eventIdx){
                            const MidiEvent&event=m_midi[trackIdx][eventIdx];
                            time+=event.time;
                            uint8_t channel=event.channel();
                            switch(event.type()){
                                case MidiEventType::note_off:
                                case MidiEventType::note_on:{
                                    m_noteMap[trackIdx].emplace_back(
                                        time,
                                        MidiTimeMode::tick,
                                        trackIdx,
                                        channel,
                                        event[1],
                                        (event.type()==MidiEventType::note_on)?event[2]:0,
                                        instruments[channel]
                                    );
                                    break;
                                }
                                case MidiEventType::key_afterTouch:{
                                    break;
                                }
                                case MidiEventType::controller:{
                                    break;
                                }
                                case MidiEventType::program:{
                                    instruments[channel]=event[1];
                                    break;
                                }
                                case MidiEventType::channel_afterTouch:{
                                    break;
                                }
                                case MidiEventType::pitchWheel:{
                                    break;
                                }
                                case MidiEventType::sysex_begin:{
                                    break;
                                }
                                case MidiEventType::sysex_end:{
                                    break;
                                }
                                case MidiEventType::meta:{
                                    switch(event[1]){
                                        case 0x01:
                                        case 0x02:
                                        case 0x03:
                                        case 0x04:
                                        case 0x05:
                                        case 0x06:
                                        case 0x07:
                                        case 0x08:
                                        case 0x09:{
                                            uint32_t _size=0;
                                            size_t beginIdx=2;
                                            for(;beginIdx<6;++beginIdx) {
                                                _size<<=7;
                                                _size|=(event[beginIdx]&0x7F);
                                                if(event[beginIdx]<0x80) {
                                                    ++beginIdx;
                                                    break;
                                                }
                                            }
                                            
                                            m_textMap[trackIdx].emplace_back(
                                                time,
                                                MidiTimeMode::tick,
                                                trackIdx,
                                                event[1],
                                                std::string(event.message.begin()+beginIdx,event.message.begin()+beginIdx+_size)
                                            );
                                            break;
                                        }

                                        case 0x51:{
                                            uint32_t mispqn=(event[3]<<16)|(event[4]<<8)|(event[5]);
                                            if(m_tempoMap[metaTrack].size()){
                                                m_tempoMap[metaTrack].emplace_back(
                                                    time,
                                                    MidiTimeMode::tick,
                                                    trackIdx,
                                                    mispqn,
                                                    m_tempoMap[metaTrack][m_tempoMap[metaTrack].size()-1].timeNode+event.time*m_tempoMap[metaTrack][m_tempoMap[metaTrack].size()-1].mispqn/m_head.tpqn()
                                                );
                                            }
                                            else{
                                                m_tempoMap[metaTrack].emplace_back(
                                                    time,
                                                    MidiTimeMode::tick,
                                                    trackIdx,
                                                    mispqn,
                                                    event.time*0x07A120u/m_head.tpqn()
                                                );
                                            }
                                            break;
                                        }
                                        case 0x58:{
                                            double newBeat,newBar;
                                            if(m_bbMap[metaTrack].size()){
                                                double deltaBeat;
                                                deltaBeat=(time-m_bbMap[metaTrack].back().time)
                                                        *m_bbMap[metaTrack].back().denominator
                                                        /4.0/m_head.tpqn();
                                                newBeat=m_bbMap[metaTrack].back().beatNode+deltaBeat;
                                                newBar=m_bbMap[metaTrack].back().barNode+deltaBeat/m_bbMap[metaTrack].back().numerator;
                                            }
                                            else{
                                                newBeat=time
                                                        *1.0/m_head.tpqn();
                                                newBar=newBeat/4;
                                            }
                                            m_bbMap[metaTrack].emplace_back(
                                                time,
                                                MidiTimeMode::tick,
                                                trackIdx,
                                                newBar,
                                                newBeat,
                                                event[3],
                                                1<<event[4]
                                            );
                                            break;
                                        }
                                        default:{
                                            break;
                                        }
                                    }
                                    break;
                                }
                                default:{
                                    break;
                                }
                            }
                        }
                    }
                    set_bb_sorted(m_noteMap);
                    return MidiErrorType::no_error;
                }
                MidiErrorType parse_micro(const MidiFile&m_midi){
                    MidiErrorType err=parse_tick(m_midi);
                    if(err!=MidiErrorType::no_error){
                        return err;
                    }
                    err=change_timeMode(MidiTimeMode::microsecond);
                    return err;
                }

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
                MidiParser(const MidiParser&_midiParser):m_head(_midiParser.m_head),m_timeMode(_midiParser.m_timeMode)
                {
                    if(m_head.format==0x00||m_head.format==0x01){
                        m_tempoMap.resize(1);
                        m_tempoMap[0]=_midiParser.m_tempoMap[0];
                        m_bbMap.resize(1);
                        m_bbMap[0]=_midiParser.m_bbMap[0];
                    }
                    else if(m_head.format==0x02){
                        m_tempoMap.resize(m_head.ntracks);
                        for(size_t i=0;i<m_head.ntracks;++i){
                            m_tempoMap[i]=_midiParser.m_tempoMap[i];
                        }
                        m_bbMap.resize(m_head.ntracks);
                        for(size_t i=0;i<m_head.ntracks;++i){
                            m_bbMap[i]=_midiParser.m_bbMap[i];
                        }
                    }
                    if(_midiParser.m_noteMap.size()){
                        m_noteMap.resize(m_head.ntracks);
                        for(size_t i=0;i<m_head.ntracks;++i){
                            m_noteMap[i]=_midiParser.m_noteMap[i];
                        }
                    }
                    if(_midiParser.m_textMap.size()){
                        m_textMap.resize(m_head.ntracks);
                        for(size_t i=0;i<m_head.ntracks;++i){
                            m_textMap[i]=_midiParser.m_textMap[i];
                        }
                    }
                    
                }
                MidiParser(MidiParser&&_midiParser):m_head(std::move(_midiParser.m_head)),m_timeMode(_midiParser.m_timeMode),m_tempoMap(std::move(_midiParser.m_tempoMap)),m_bbMap(std::move(_midiParser.m_bbMap)),m_noteMap(std::move(_midiParser.m_noteMap)),m_textMap(std::move(_midiParser.m_textMap))
                {}
                MidiParser(const MidiFile&_midi,MidiTimeMode _timeMode=MidiTimeMode::tick):m_timeMode(_timeMode){
                    if(_midi.is_read_success()){
                        m_head=_midi.head;
                        if(m_head.format==0x00||m_head.format==0x01){
                            m_tempoMap.resize(1);
                            m_bbMap.resize(1);
                        }
                        else if(m_head.format==0x02){
                            m_tempoMap.resize(m_head.ntracks);
                            m_bbMap.resize(m_head.ntracks);
                        }
                        m_noteMap.resize(m_head.ntracks);
                        m_textMap.resize(m_head.ntracks);

                        if(_timeMode==MidiTimeMode::tick){
                            parse_tick(_midi);
                        }
                        else if(_timeMode==MidiTimeMode::microsecond){
                            parse_micro(_midi);
                        }
                    }
                    else{

                    }
                }
                MidiParser(MidiFile&&_midi,MidiTimeMode _timeMode=MidiTimeMode::tick):m_timeMode(_timeMode){
                    if(_midi.is_untouched()){
                        _midi.read();
                    }
                    if(_midi.is_read_success()){
                        m_head=_midi.head;
                        if(m_head.format==0x00||m_head.format==0x01){
                            m_tempoMap.resize(1);
                            m_bbMap.resize(1);
                        }
                        else if(m_head.format==0x02){
                            m_tempoMap.resize(m_head.ntracks);
                            m_bbMap.resize(m_head.ntracks);
                        }
                        m_noteMap.resize(m_head.ntracks);
                        m_textMap.resize(m_head.ntracks);

                        if(_timeMode==MidiTimeMode::tick){
                            parse_tick(_midi);
                        }
                        else if(_timeMode==MidiTimeMode::microsecond){
                            parse_micro(_midi);
                        }
                    }
                    else{

                    }
                }
                MidiParser(std::string _filename,MidiTimeMode _timeMode=MidiTimeMode::tick):
                    m_timeMode(_timeMode){
                    MidiFile _midi(_filename);
                    _midi.read();
                    *this=MidiParser(_midi,_timeMode);
                }
                ~MidiParser(void){
                }
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
                MidiErrorType change_timeMode(MidiTimeMode _mode=MidiTimeMode::microsecond){
                    if(m_timeMode==_mode){
                        return MidiErrorType::no_error;
                    }
                    MidiErrorType err=MidiErrorType::no_error;
                    if(m_timeMode==MidiTimeMode::tick&&_mode==MidiTimeMode::microsecond){
                        err=change_timeMode_tick2micro_sorted(m_noteMap);
                        if(err!=MidiErrorType::no_error){
                            return err;
                        }
                        err=change_timeMode_tick2micro_sorted(m_bbMap);
                        if(err!=MidiErrorType::no_error){
                            return err;
                        }
                        err=change_timeMode_tick2micro_sorted(m_textMap);
                        if(err!=MidiErrorType::no_error){
                            return err;
                        }
                    }
                    else if(m_timeMode==MidiTimeMode::microsecond&&_mode==MidiTimeMode::tick){
                        err=change_timeMode_micro2tick_sorted(m_noteMap);
                        if(err!=MidiErrorType::no_error){
                            return err;
                        }
                        err=change_timeMode_micro2tick_sorted(m_bbMap);
                        if(err!=MidiErrorType::no_error){
                            return err;
                        }
                        err=change_timeMode_micro2tick_sorted(m_textMap);
                        if(err!=MidiErrorType::no_error){
                            return err;
                        }
                    }
                    for(uint8_t trackIdx=0;trackIdx<m_tempoMap.size();++trackIdx){
                        for(size_t eventIdx=0;eventIdx<m_tempoMap[trackIdx].size();++eventIdx){
                            m_tempoMap[trackIdx][eventIdx].timeMode=_mode;
                            std::swap(m_tempoMap[trackIdx][eventIdx].time,m_tempoMap[trackIdx][eventIdx].timeNode);
                        }
                    }
                    m_timeMode=_mode;
                    return err;
                }
            public:
                uint16_t format(void)const{
                    return m_head.format;
                }
                uint16_t ntracks(void)const {
                    return m_head.ntracks;
                }
                uint16_t division(void)const{
                    return m_head.division;
                }
                uint16_t tpqn(void)const{
                    return m_head.tpqn();
                }
                const NoteMap&noteMap(void)const{
                    return m_noteMap;
                }
                const BarBeatMap&bbMap(void)const{
                    return m_bbMap;
                }
                const TempoMap&tempoMap(void)const{
                    return m_tempoMap;
                }
                const TextMap&textMap(void)const{
                    return m_textMap;
                }

                MidiTimeMode timeMode(void)const{
                    return m_timeMode;
                }
            public:
                MidiParser& operator=(const MidiParser&)=default;
                MidiParser& operator=(MidiParser&&)=default;
            
            friend class MidiShortMessageList;
        };
    }
}
#endif