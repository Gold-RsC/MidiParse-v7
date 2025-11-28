/********************************************************
    Name: MidiFun.hpp
    Copyright: 2024 Csrua
    Author: Csrua
    website: https://space.bilibili.com/361846321
    contact: (QQ prioritized)
        bilibili: Csrua
        QQ:       310106329
        email:    310106329@qq.com
    Date: 2024-07-30
    Description: This head file provides functions which
        can parse midi files into various types of data.
        When using the file, you must CLEARLY indicate the
        AUTHOR(Csrua) and the ORIGIN of the file.
********************************************************/

#ifndef MIDIFUN_HPP
#define MIDIFUN_HPP
#include"MidiFile.hpp"
#include"MoreMidiEvent.hpp"

namespace GoldType{
    namespace MidiParse{
        /********************************
                    Midi Function
        ********************************/
        class MidiFun{
            protected:
                MidiFile m_midi;
                TempoList*m_timeMap;
                BarBeatList m_bbMap;
                NoteList m_noteList;
                LyricList m_lyricList;
                TsList m_tsList;
                ProgramList m_programList;
            private:
                uint16_t get_tempoTrack(ushort _track)const {
                    if(m_midi.format()==2){
				        return _track;
                    }
                    else if(m_midi.format()==0||m_midi.format()==1){
                        return 0;
                    }
                    //error
                    return _track;
                }
				void build_map(void){
				    if(m_midi.format()==2){
				        m_timeMap=new TempoList[m_midi.ntracks()];
                    }
                    else if(m_midi.format()==0||m_midi.format()==1){
                        m_timeMap=new TempoList[1];
                    }
                    
    				for(size_t trackIdx=0;trackIdx<m_midi.ntracks();++trackIdx){
                        uint64_t time=0;
                        uint64_t lastTime=0;
                        double lastBar=0;
                        double lastBeat=0;
                        uint8_t lastN=4;
                        uint8_t lastD=2;
                        for(size_t eventIdx=0;eventIdx<m_midi[trackIdx].size();++eventIdx){
                            time+=m_midi[trackIdx][eventIdx].time();
                            if(m_midi[trackIdx][eventIdx].is_meta()&&m_midi[trackIdx][eventIdx][1]==0x51){
                                uint16_t tk=get_tempoTrack(trackIdx);
                                m_timeMap[tk].emplace_back(
									time,
									trackIdx,
                                    (m_midi[trackIdx][eventIdx][3]<<16)|(m_midi[trackIdx][eventIdx][4]<<8)|(m_midi[trackIdx][eventIdx][5])
								);
                            }
                            else if(m_midi[trackIdx][eventIdx].is_meta()&&m_midi[trackIdx][eventIdx][1]==0x58){
                                double deltaBeat=1.0*(time-lastTime)*(1<<lastD)/4/m_midi.tpqn();
                                double deltaBar=deltaBeat/lastN;
                                double beat=lastBeat+deltaBeat;
                                double bar=lastBar+deltaBar;
                                m_bbMap.emplace_back(
									time,
									trackIdx,
                                    bar,
                                    beat,
                                    m_midi[trackIdx][eventIdx][3],
                                    1<<m_midi[trackIdx][eventIdx][4]
								);
                                lastTime=time;
                                lastBar=bar;
                                lastBeat=beat;
                                lastN=m_midi[trackIdx][eventIdx][3];
                                lastD=m_midi[trackIdx][eventIdx][4];
                            }
                        }
                    }改到这里了！ 
                    if(m_midi.format()==2){
				        for(size_t trackIdx=0;trackIdx<m_midi.ntracks();++trackIdx){
                            for(TempoList::iterator it=m_timeMap[trackIdx].begin();it!=m_timeMap[trackIdx].end();++it){
                                if(it==m_timeMap[trackIdx].begin()){
                                    it->timeNode()=it->time()*0x07A120/m_midi.tpqn();
                                }
                                else{
                                    it->timeNode()=(it->time()-(it-1)->time())*((it-1)->mispqn())/m_midi.tpqn();
                                    it->timeNode()+=(it-1)->timeNode();
                                }
                            }
                        }
                    }
                    else if(m_midi.format()==0||m_midi.format()==1){
                        for(TempoList::iterator it=m_timeMap[0].begin();it!=m_timeMap[0].end();++it){
                            if(it==m_timeMap[0].begin()){
                                it->timeNode()=it->time()*0x07A120/m_midi.tpqn();
                            }
                            else{
                                it->timeNode()=(it->time()-(it-1)->time())*((it-1)->mispqn())/m_midi.tpqn();
                                it->timeNode()+=(it-1)->timeNode();
                            }
                        }
                    }
				}
				uint16_t find_timeMapIdx(uint64_t _tick,uint16_t _track=0xFFFF)const {
				    uint16_t tk=get_tempoTrack(_track);
			        if(_tick<m_timeMap[tk][0].time()){
			            return 0;
                    }
                    else if(_tick>=m_timeMap[tk][m_timeMap[tk].size()-1].time()){
                        return m_timeMap[tk].size();
                    }
			        uint16_t left=0,right=m_timeMap[tk].size()-1,mid=m_timeMap[tk].size()/2;
			        while(left<=right){
			            if(m_timeMap[tk][mid].time()<=_tick&&m_timeMap[tk][mid+1].time()>_tick){
			                return mid+1;
                        }
                        else if(m_timeMap[tk][mid].time()>_tick){
                            right=mid-1;
                        }
                        else{
                            left=mid+1;
                        }
			            mid=(left+right)/2;
                    }
                    return 0xFFFF;
                }
                ushort find_bbMapIdx(ull _tick)const {
                    if(_tick<m_bbMap[0].time()){
			            return 0xFFFF;
                    }
                    else if(_tick>=m_bbMap[m_bbMap.size()-1].time()){
                        return m_bbMap.size()-1;
                    }
                    ushort left=0,right=m_bbMap.size()-1,mid=m_bbMap.size()/2;
                    while(left<=right){
                        if(m_bbMap[mid].time()<=_tick&&m_bbMap[mid+1].time()>_tick){
                            return mid;
                        }
                        else if(m_bbMap[mid].time()>_tick){
                            right=mid-1;
                        }
                        else{
                            left=mid+1;
                        }
                        mid=(left+right)/2;
                    }
                    return 0xFFFF;
                }
                
                
//                #ifdef MIDI_DEBUG
                MidiErrorType write_error(std::string _text)const {
                    midierror.write_text("MidiFunctionError:\n\t");
                    midierror.write_text(_text);
                }
//                #endif
            public:
                MidiFun(void)=delete;
                MidiFun(const MidiFun&)=delete;
                MidiFun(std::string _filename):
                    m_midi(_filename){
                    m_midi.read();
                    build_map();
                    uchar instrumentList[16];
                    for(size_t i=0;i<16;++i){
                        instrumentList[i]=0;
                    }
                    for(size_t trackIdx=0;trackIdx<m_midi.ntracks();++trackIdx){
                        ull time=0;
                        for(size_t eventIdx=0;eventIdx<m_midi[trackIdx].size();++eventIdx){
                            time+=m_midi[trackIdx][eventIdx].time();
                            switch(m_midi[trackIdx][eventIdx].type()&0xF0){
								case (uchar)MidiEventType::note_off:
								case (uchar)MidiEventType::note_on:{
								    std::pair<double,double> bb=match_barAndBeat(time);
									m_noteList.emplace_back(time,
										trackIdx,
										m_midi[trackIdx][eventIdx].type()&0x0F,
										m_midi[trackIdx][eventIdx][1],
										(m_midi[trackIdx][eventIdx].type()&0xF0)==(uchar)MidiEventType::note_off?0:m_midi[trackIdx][eventIdx][2],
										instrumentList[m_midi[trackIdx][eventIdx].type()&0x0F],
                                        bb.first,
                                        bb.second);
									break;
								}
                            	case (uchar)MidiEventType::program:{
                            		instrumentList[m_midi[trackIdx][eventIdx].type()&0x0F]=m_midi[trackIdx][eventIdx][1];
                            		m_programList.emplace_back(time,trackIdx,m_midi[trackIdx][eventIdx].type()&0x0F,m_midi[trackIdx][eventIdx][1]);
									break;
								}
								case (uchar)MidiEventType::pitchWheel:{
									
									break;
								}
								case 0xF0:{
									switch(m_midi[trackIdx][eventIdx].type()){
										case (uchar)MidiEventType::meta:{
											switch(m_midi[trackIdx][eventIdx][1]){
												case 0x05:{
													m_lyricList.emplace_back(time,trackIdx,std::string(m_midi[trackIdx][eventIdx].message().begin()+m_midi[trackIdx][eventIdx].get_dataBeginIdx(),m_midi[trackIdx][eventIdx].message().end()));
													break;
												}
												case 0x58:{
													m_tsList.emplace_back(time,trackIdx,m_midi[trackIdx][eventIdx][3],1<<m_midi[trackIdx][eventIdx][4],m_midi[trackIdx][eventIdx][5],m_midi[trackIdx][eventIdx][6]);
													break;
												}
											}
											break;
										}
										case (uchar)MidiEventType::sysex_begin:
										case (uchar)MidiEventType::sysex_end:{
											
											break;
										}
									}
									break;
								}
							}
                        }
                    }
                }
                ~MidiFun(void){
                	delete[] m_timeMap; 
                	m_timeMap=nullptr; 
				}
                
				template<typename _FUN,typename...Args>
                void for_track(_FUN fun,Args...args){
                    for(size_t trackIdx=0;trackIdx<m_midi.ntracks();++trackIdx){
                        fun(m_midi[trackIdx],args...);
                    }
                }
                template<typename _FUN,typename...Args>
                void for_event(_FUN fun,Args...args){
                    for(size_t trackIdx=0;trackIdx<m_midi.ntracks();++trackIdx){
                        for(size_t eventIdx=0;eventIdx<m_midi[trackIdx].size();++eventIdx){
                            fun(m_midi[trackIdx][eventIdx],args...);
                        }
                    }
                }
                
                MidiFile&midi(void){
                    return m_midi;
                }
                const MidiFile&midi(void)const {
                    return m_midi;
                }
				NoteList&noteList(void){
                    return m_noteList;
                }
                const NoteList&noteList(void)const{
                    return m_noteList;
                }
                ProgramList&programList(void){
                    return m_programList;
                }
                const ProgramList&programList(void)const {
                    return m_programList;
                }
                LyricList&lyricList(void){
                    return m_lyricList;
                }
                const LyricList&lyricList(void)const {
                    return m_lyricList;
                }
                TempoList*timeMap(void){
                    return m_timeMap;
                }
                const TempoList*timeMap(void)const{
                    return m_timeMap;
                }
                TsList&tsList(void){
                    return m_tsList;
                }
                const TsList&tsList(void)const {
                    return m_tsList;
                }
                
                ull match_microSecond(ull _tick,ushort _track=0xFFFF)const {
                    ushort idx=find_timeMapIdx(_tick,_track);
                    if(idx==0){
                        return _tick*0x07A120/m_midi.tpqn();
                    }
                    else if(idx==0xFFFF){
                        return 0xFFFFFFFFFFFFFFFF;
                    }
                    ushort tk=get_tempoTrack(_track);
                    return m_timeMap[tk][idx-1].timeNode()+(_tick-m_timeMap[tk][idx-1].time())*m_timeMap[tk][idx-1].mispqn()/m_midi.tpqn();
                }
                std::pair<double,double> match_barAndBeat(ull _tick)const {
                    ushort idx=find_bbMapIdx(_tick);
                    if(idx==0xFFFF){
                        return std::pair<double,double>(1.0,1.0);
                    }
                    double deltaBeat=1.0*(_tick-m_bbMap[idx].time())*m_bbMap[idx].denominator()/4/m_midi.tpqn();
                    double deltaBar=deltaBeat/m_bbMap[idx].numerator();
                    double beat=m_bbMap[idx].beatNode()+deltaBeat;
                    double bar=m_bbMap[idx].barNode()+deltaBar;
                    return std::pair<double,double>(bar,beat);
                }
                
                
                
                template<typename _MidiEvent>
                void change_oneTimeState(MidiEventList<_MidiEvent>&li,TimeState _timeState=TimeState::microSecond){
                    if(li.timeState()==TimeState::tick&&_timeState==TimeState::microSecond){
                        for(typename MidiEventList<_MidiEvent>::iterator it=li.begin();it!=li.end();++it){
                            it->time()=match_microSecond(it->time(),it->track());
                        }
                    }
                    li.timeState()=_timeState;
                }
                void change_allTimeState(TimeState _timeState=TimeState::microSecond){
                    change_oneTimeState(m_noteList,_timeState);
                    change_oneTimeState(m_lyricList,_timeState);
                    change_oneTimeState(m_tsList,_timeState);
                    change_oneTimeState(m_programList,_timeState);
                }
                
                MidiErrorType write_noteList(FILE*_file,std::string _formatStr)const {
                    if(!m_midi.track()){
//        				#ifdef MIDI_DEBUG
                        return write_error("The function write_noteList() cannot run susseecefully!\n");
//                        #else
//                        return midierror.get_type();
//                        #endif
					}
					
                    uint _size=_formatStr.length();
                    for(uint i=0;i<_size;++i){
                        if(_formatStr[i]=='@'&&i+1<_size){
                            ++i;
                            switch(_formatStr[i]){
                                case 't':{
                                    fprintf(_file,"time");
                                    break;
                                }
                                case 'r':{
                                    fprintf(_file,"track");
                                    break;
                                }
                                
                                case 'c':{
                                    fprintf(_file,"channel");
                                    break;
                                }
                                case 'i':{
                                    fprintf(_file,"instrument");
                                    break;
                                }
                                case 'p':{
                                    fprintf(_file,"pitch");
                                    break;
                                }
                                case 'v':{
                                    fprintf(_file,"velocity");
                                    break;
                                }
                                case 'B':{
                                    fprintf(_file,"bar");
                                    break;
                                }
                                case 'b':{
                                    fprintf(_file,"beat");
                                    break;
                                }
                                default:{
                                    fputc('@',_file);
                                    fputc(_formatStr[i],_file);
                                    break;
                                }
                            }
                        }
                        else{
                            fputc(_formatStr[i],_file);
                        }
                    }
                    for(NoteList::const_iterator it=m_noteList.cbegin();it!=m_noteList.cend();++it){
                        fputc('\n',_file);
                        for(uint i=0;i<_size;++i){
                            if(_formatStr[i]=='@'&&i+1<_size){
                                ++i;
                                switch(_formatStr[i]){
                                    case 't':{
                                        fprintf(_file,"%llu",it->time());
                                        break;
                                    }
                                    case 'r':{
                                        fprintf(_file,"%hu",it->track());
                                        break;
                                    }
                                    
                                    case 'c':{
                                        fprintf(_file,"%2.2X",it->channel());
                                        break;
                                    }
                                    case 'i':{
                                        fprintf(_file,"%hu",it->instrument());
                                        break;
                                    }
                                    case 'p':{
                                        fprintf(_file,"%2.2X",it->pitch());
                                        break;
                                    }
                                    case 'v':{
                                        fprintf(_file,"%2.2X",it->velocity());
                                        break;
                                    }
                                    case 'B':{
                                        fprintf(_file,"%.6f",it->bar());
                                        break;
                                    }
                                    case 'b':{
                                        fprintf(_file,"%.6f",it->beat());
                                        break;
                                    }
                                    default:{
                                        fputc('@',_file);
                                        fputc(_formatStr[i],_file);
                                        break;
                                    }
                                }
                            }
                            else{
                                fputc(_formatStr[i],_file);
                            }
                        }
                    }
                    return MidiErrorType::no_error;
                }
				
            public:
                MidiFun&operator=(const MidiFun&)=delete;
                
                friend class ShortMessageList;
                friend MidiErrorType play(std::string);
                
        };
    }
}

#endif
