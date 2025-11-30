#include"Tempo.hpp"
namespace GoldType{
    namespace MidiParse{
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
    }
}
