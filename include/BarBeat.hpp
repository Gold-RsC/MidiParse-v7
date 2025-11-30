/********************************************************************************************************
 * File Name    : BarBeat.hpp
 * Author       : Csrua / Gold_RsC
 * github       : Gold-RsC(https://github.com/Gold-RsC)
 * bilibili     : Csrua(https://space.bilibili.com/361846321)
 * Email        : 310106329@qq.com
 * Create Date  : 2020/04/26
 * Update Date  : 2025/12/01
 * Details      : README.md 
 ********************************************************************************************************/
#ifndef BARBEAT_HPP
#define BARBEAT_HPP
#include"BasicMidiEvent.hpp"

namespace GoldType{
    namespace MidiParse{
        class BarBeat:public BasicMidiEvent_Meta{
            public:
                double barNode;
                double beatNode;
                uint8_t numerator;
                uint8_t denominator;
            public:
                BarBeat(uint64_t _time=0,MidiTimeMode _timeMode=MidiTimeMode::tick,uint8_t _track=0,double _barNode=0,double _beatNode=0,uint8_t _numerator=4,uint8_t _denominator=4);
                BarBeat(const BarBeat&)=default;
                ~BarBeat(void)=default;
        };
        bool operator==(const BarBeat&a,const BarBeat&b);
        bool operator!=(const BarBeat&a,const BarBeat&b);
        bool operator<(const BarBeat&a,const BarBeat&b);
        bool operator>(const BarBeat&a,const BarBeat&b);
        bool operator<=(const BarBeat&a,const BarBeat&b);
        bool operator>=(const BarBeat&a,const BarBeat&b);
        
        using BarBeatList=MidiEventList<BarBeat>;

        using BarBeatMap=MidiEventMap<BarBeat>;
    }
}
#endif
