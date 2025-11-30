#include"Text.hpp"
namespace GoldType{
    namespace MidiParse{
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
    }
}
