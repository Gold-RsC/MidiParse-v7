#include"Program.hpp"
namespace GoldType{
    namespace MidiParse{
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
    }
}
