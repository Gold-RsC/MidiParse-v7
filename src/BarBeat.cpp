#include"BarBeat.hpp"
namespace GoldType{
    namespace MidiParse{
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