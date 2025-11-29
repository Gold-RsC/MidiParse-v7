#include"MoreMidiEvent.hpp"
namespace GoldType{
    namespace MidiParse{
        Note::Note(uint64_t _time,MidiTimeMode _timeMode,uint8_t _track,uint8_t _channel,uint8_t _pitch,uint8_t _velocity,uint8_t _instrument,double _bar,double _beat):
            BasicMidiEvent_Non(_time,_timeMode,_track,_channel),pitch(_pitch),velocity(_velocity),instrument(_instrument),bar(_bar),beat(_beat){}
        bool Note::is_empty(void)const {
            return !(time)&&!(channel)&&!(track)&&!(pitch)&&!(velocity)&&(!instrument);
        }
        bool operator==(const Note&a,const Note&b){
            return a.time==b.time&&
            a.timeMode==b.timeMode&&
            a.channel==b.channel&&
            a.track==b.track&&
            a.pitch==b.pitch&&
            a.velocity==b.velocity&&
            a.instrument==b.instrument;
        }
        bool operator!=(const Note&a,const Note&b){
            return !(a==b);
        }
        bool operator<(const Note&a,const Note&b){
            if(a.timeMode!=b.timeMode){
                return a.timeMode<b.timeMode;
            }
            if(a.time!=b.time){
                return a.time<b.time;
            }
            if(a.channel!=b.channel){
                return a.channel<b.channel;
            }
            if(a.track!=b.track){
                return a.track<b.track;
            }
            if(a.velocity!=b.velocity){
                return a.velocity<b.velocity;
            }
            if(a.pitch!=b.pitch){
                return a.pitch<b.pitch;
            }
            return a.instrument<b.instrument;
        }
        bool operator>(const Note&a,const Note&b){
            if(a.timeMode!=b.timeMode){
                return a.timeMode>b.timeMode;
            }
            if(a.time!=b.time){
                return a.time>b.time;
            }
            if(a.channel!=b.channel){
                return a.channel>b.channel;
            }
            if(a.track!=b.track){
                return a.track>b.track;
            }
            if(a.pitch!=b.pitch){
                return a.pitch>b.pitch;
            }
            if(a.velocity!=b.velocity){
                return a.velocity>b.velocity;
            }
            return a.instrument>b.instrument;
        }
        bool operator<=(const Note&a,const Note&b){
            return !(a>b);
        }
        bool operator>=(const Note&a,const Note&b){
            return !(a<b);
        }
        
        Program::Program(uint64_t _time,MidiTimeMode _timeMode,uint8_t _track,uint8_t _channel,uint8_t _instrument):
            BasicMidiEvent_Non(_time,_timeMode,_track,_channel),instrument(_instrument){}
        bool operator==(const Program&a,const Program&b){
            return a.time==b.time&&
            a.timeMode==b.timeMode&&
            a.channel==b.channel&&a.track==b.track&&a.instrument==b.instrument;
        }
        bool operator!=(const Program&a,const Program&b){
            return !(a==b);
        }
        bool operator<(const Program&a,const Program&b){
            if(a.timeMode!=b.timeMode){
                return a.timeMode<b.timeMode;
            }
            if(a.time!=b.time){
                return a.time<b.time;
            }
            if(a.channel!=b.channel){
                return a.channel<b.channel;
            }
            if(a.track!=b.track){
                return a.track<b.track;
            }
            return a.instrument<b.instrument;
        }
        bool operator>(const Program&a,const Program&b){
            if(a.timeMode!=b.timeMode){
                return a.timeMode>b.timeMode;
            }
            if(a.time!=b.time){
                return a.time>b.time;
            }
            if(a.channel!=b.channel){
                return a.channel>b.channel;
            }
            if(a.track!=b.track){
                return a.track>b.track;
            }
            return a.instrument>b.instrument;
        }
        bool operator<=(const Program&a,const Program&b){
            return !(a>b);
        }
        bool operator>=(const Program&a,const Program&b){
            return !(a<b);
        }
        
        Text::Text(uint64_t _time,MidiTimeMode _timeMode,uint8_t _track,uint8_t _type,const std::string&_text):
            BasicMidiEvent_Meta(_time,_timeMode,_track),type(_type),text(_text){}
        bool operator==(const Text&a,const Text&b){
            return a.time==b.time&&
            a.timeMode==b.timeMode&&
            a.track==b.track&&
            a.type==b.type&&
            a.text==b.text;
        }
        bool operator!=(const Text&a,const Text&b){
            return !(a==b);
        }
        bool operator<(const Text&a,const Text&b){
            if(a.timeMode!=b.timeMode){
                return a.timeMode<b.timeMode;
            }
            if(a.time!=b.time){
                return a.time<b.time;
            }
            if(a.track!=b.track){
                return a.track<b.track;
            }
            if(a.type!=b.type){
                return a.type<b.type;
            }
            return a.text<b.text;
        }
        bool operator>(const Text&a,const Text&b){
            if(a.timeMode!=b.timeMode){
                return a.timeMode>b.timeMode;
            }
            if(a.time!=b.time){
                return a.time>b.time;
            }
            if(a.track!=b.track){
                return a.track>b.track;
            }
            if(a.type!=b.type){
                return a.type>b.type;
            }
            return a.text>b.text;
        }
        bool operator<=(const Text&a,const Text&b){
            return !(a>b);
        }
        bool operator>=(const Text&a,const Text&b){
            return !(a<b);
        }
        
        Tempo::Tempo(uint64_t _time,MidiTimeMode _timeMode,uint8_t _track,uint32_t _mispqn,uint64_t _timeNode):
            BasicMidiEvent_Meta(_time,_timeMode,_track),mispqn(_mispqn),timeNode(_timeNode){}
        double Tempo::bpm(void)const{
            return 6e7/mispqn;
        }
        uint64_t&Tempo::tick(void){
            if(timeMode==MidiTimeMode::tick){
                return time;
            }
            return timeNode;
        }
        const uint64_t&Tempo::tick(void)const {
            if(timeMode==MidiTimeMode::tick){
                return time;
            }
            return timeNode;
        }
        uint64_t&Tempo::microsecond(void){
            if(timeMode==MidiTimeMode::microsecond){
                return time;
            }
            return timeNode;
        }
        const uint64_t&Tempo::microsecond(void)const {
            if(timeMode==MidiTimeMode::microsecond){
                return time;
            }
            return timeNode;
        }
        bool operator==(const Tempo&a,const Tempo&b){
            return a.timeMode==b.timeMode&&a.time==b.time&&a.track==b.track&&a.mispqn==b.mispqn;
        }
        bool operator!=(const Tempo&a,const Tempo&b){
            return !(a==b);
        }
        bool operator<(const Tempo&a,const Tempo&b){
            if(a.timeMode!=b.timeMode){
                return a.timeMode<b.timeMode;
            }
            if(a.time!=b.time){
                return a.time<b.time;
            }
            if(a.track!=b.track){
                return a.track<b.track;
            }
            return a.mispqn<b.mispqn; 
        }
        bool operator>(const Tempo&a,const Tempo&b){
            if(a.timeMode!=b.timeMode){
                return a.timeMode>b.timeMode;
            }
            if(a.time!=b.time){
                return a.time>b.time;
            }
            if(a.track!=b.track){
                return a.track>b.track;
            }
            return a.mispqn>b.mispqn; 
        }
        bool operator<=(const Tempo&a,const Tempo&b){
            return !(a>b);
        }
        bool operator>=(const Tempo&a,const Tempo&b){
            return !(a<b);
        }

        TimeSignature::TimeSignature(uint64_t _time,MidiTimeMode _timeMode,uint8_t _track,uint8_t _numerator,uint8_t _denominator,uint8_t _tickPerMidiclock,uint8_t _num32ndNotePer24Midiclock):
            BasicMidiEvent_Meta(_time,_timeMode,_track),numerator(_numerator),denominator(_denominator),tickPerMidiclock(_tickPerMidiclock),num32ndNotePer24Midiclock(_num32ndNotePer24Midiclock){}
        bool operator==(const TimeSignature&a,const TimeSignature&b){
            return a.time==b.time&&
            a.timeMode==b.timeMode&&
            a.track==b.track&&a.numerator==b.numerator&&a.denominator==b.denominator&&a.tickPerMidiclock==b.tickPerMidiclock&&a.num32ndNotePer24Midiclock==b.num32ndNotePer24Midiclock;
        }
        bool operator!=(const TimeSignature&a,const TimeSignature&b){
            return !(a==b);
        }
        bool operator<(const TimeSignature&a,const TimeSignature&b){
            if(a.timeMode!=b.timeMode){
                return a.timeMode<b.timeMode;
            }
            if(a.time!=b.time){
                return a.time<b.time;
            }
            if(a.track!=b.track){
                return a.track<b.track;
            }
            if(a.numerator!=b.numerator){
                return a.numerator<b.numerator; 
            }
            if(a.denominator!=b.denominator){
                return a.denominator<b.denominator; 
            }
            if(a.tickPerMidiclock!=b.tickPerMidiclock){
                return a.tickPerMidiclock<b.tickPerMidiclock; 
            }
            return a.num32ndNotePer24Midiclock<b.num32ndNotePer24Midiclock;
        }
        bool operator>(const TimeSignature&a,const TimeSignature&b){
            if(a.timeMode!=b.timeMode){
                return a.timeMode>b.timeMode;
            }
            if(a.time!=b.time){
                return a.time>b.time;
            }
            if(a.track!=b.track){
                return a.track>b.track;
            }
            if(a.numerator!=b.numerator){
                return a.numerator>b.numerator; 
            }
            if(a.denominator!=b.denominator){
                return a.denominator>b.denominator; 
            }
            if(a.tickPerMidiclock!=b.tickPerMidiclock){
                return a.tickPerMidiclock>b.tickPerMidiclock; 
            }
            return a.num32ndNotePer24Midiclock>b.num32ndNotePer24Midiclock;
        }
        bool operator<=(const TimeSignature&a,const TimeSignature&b){
            return !(a>b);
        }
        bool operator>=(const TimeSignature&a,const TimeSignature&b){
            return !(a<b);
        }
        
        BarBeat::BarBeat(uint64_t _time,MidiTimeMode _timeMode,uint8_t _track,double _barNode,double _beatNode,uint8_t _numerator,uint8_t _denominator):
            BasicMidiEvent_Meta(_time,_timeMode,_track),barNode(_barNode),beatNode(_beatNode),numerator(_numerator),denominator(_denominator){}
        bool operator==(const BarBeat&a,const BarBeat&b){
            return a.time==b.time&&
            a.timeMode==b.timeMode&&
            a.track==b.track&&a.barNode==b.barNode&&a.barNode==b.barNode;
        }
        bool operator!=(const BarBeat&a,const BarBeat&b){
            return !(a==b);
        }
        bool operator<(const BarBeat&a,const BarBeat&b){
            if(a.timeMode!=b.timeMode){
                return a.timeMode<b.timeMode;
            }
            if(a.time!=b.time){
                return a.time<b.time;
            }
            if(a.track!=b.track){
                return a.track<b.track;
            }
            if(a.barNode!=b.barNode){
                return a.barNode<b.barNode; 
            }
            if(a.barNode!=b.barNode){
                return a.barNode<b.barNode; 
            }
            if(a.numerator!=b.numerator){
                return a.numerator<b.numerator;
            }
            return a.denominator<b.denominator;
        }
        bool operator>(const BarBeat&a,const BarBeat&b){
            if(a.timeMode!=b.timeMode){
                return a.timeMode>b.timeMode;
            }
            if(a.time!=b.time){
                return a.time>b.time;
            }
            if(a.track!=b.track){
                return a.track>b.track;
            }
            if(a.barNode!=b.barNode){
                return a.barNode>b.barNode; 
            }
            if(a.barNode!=b.barNode){
                return a.barNode>b.barNode; 
            }
            if(a.numerator!=b.numerator){
                return a.numerator>b.numerator;
            }
            return a.denominator>b.denominator;
        }
        bool operator<=(const BarBeat&a,const BarBeat&b){
            return !(a>b);
        }
        bool operator>=(const BarBeat&a,const BarBeat&b){
            return !(a<b);
        }
        
    }
}