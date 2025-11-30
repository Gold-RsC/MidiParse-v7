#include"TimeSignature.hpp"
namespace GoldType{
    namespace MidiParse{
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
    }
}