#include"Note.hpp"
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
    }
}
