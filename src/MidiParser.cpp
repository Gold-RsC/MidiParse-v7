#include"MidiParser.hpp"
namespace GoldType{
    namespace MidiParse{
        uint8_t MidiParser::get_metaTrack(uint8_t _track)const{
            if(m_head.format==0||m_head.format==0x01){
                return 0;
            }
            else if(m_head.format==0x02){
                return _track;
            }
            return 0xFF;
        }
        
        MidiErrorType MidiParser::parse_tick(const MidiFile&m_midi){
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
        MidiErrorType MidiParser::parse_micro(const MidiFile&m_midi){
            MidiErrorType err=parse_tick(m_midi);
            if(err!=MidiErrorType::no_error){
                return err;
            }
            err=change_timeMode(MidiTimeMode::microsecond);
            return err;
        }

        MidiParser::MidiParser(const MidiParser&_midiParser):m_head(_midiParser.m_head),m_timeMode(_midiParser.m_timeMode){
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
        MidiParser::MidiParser(MidiParser&&_midiParser):m_head(std::move(_midiParser.m_head)),m_timeMode(_midiParser.m_timeMode),m_tempoMap(std::move(_midiParser.m_tempoMap)),m_bbMap(std::move(_midiParser.m_bbMap)),m_noteMap(std::move(_midiParser.m_noteMap)),m_textMap(std::move(_midiParser.m_textMap))
        {}
        MidiParser::MidiParser(const MidiFile&_midi,MidiTimeMode _timeMode):m_timeMode(_timeMode){
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
        MidiParser::MidiParser(MidiFile&&_midi,MidiTimeMode _timeMode):m_timeMode(_timeMode){
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
        MidiParser::MidiParser(std::string _filename,MidiTimeMode _timeMode):
            m_timeMode(_timeMode){
            MidiFile _midi(_filename);
            _midi.read();
            *this=MidiParser(_midi,_timeMode);
        }
    
        MidiErrorType MidiParser::change_timeMode(MidiTimeMode _mode){
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
    
        uint16_t MidiParser::format(void)const{
            return m_head.format;
        }
        uint16_t MidiParser::ntracks(void)const {
            return m_head.ntracks;
        }
        uint16_t MidiParser::division(void)const{
            return m_head.division;
        }
        uint16_t MidiParser::tpqn(void)const{
            return m_head.tpqn();
        }
        const NoteMap&MidiParser::noteMap(void)const{
            return m_noteMap;
        }
        const BarBeatMap&MidiParser::bbMap(void)const{
            return m_bbMap;
        }
        const TempoMap&MidiParser::tempoMap(void)const{
            return m_tempoMap;
        }
        const TextMap&MidiParser::textMap(void)const{
            return m_textMap;
        }

        MidiTimeMode MidiParser::timeMode(void)const{
            return m_timeMode;
        }
    }
}