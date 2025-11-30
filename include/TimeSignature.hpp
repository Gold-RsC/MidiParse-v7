/********************************************************************************************************
 * File Name    : TimeSignature.hpp
 * Author       : Csrua / Gold_RsC
 * github       : Gold-RsC(https://github.com/Gold-RsC)
 * bilibili     : Csrua(https://space.bilibili.com/361846321)
 * Email        : 310106329@qq.com
 * Create Date  : 2020/04/26
 * Update Date  : 2025/12/01
 * Details      : README.md 
 ********************************************************************************************************/
#ifndef TIMESIGNATURE_HPP
#define TIMESIGNATURE_HPP

#include"BasicMidiEvent.hpp"
#include<string>
#include<map>
#include<queue>
namespace GoldType{
    namespace MidiParse{

        class TimeSignature:public BasicMidiEvent_Meta{
            public:
                uint8_t numerator;
                uint8_t denominator;
                uint8_t tickPerMidiclock;
                uint8_t num32ndNotePer24Midiclock;
            public:
                TimeSignature(uint64_t _time=0,MidiTimeMode _timeMode=MidiTimeMode::tick,uint8_t _track=0,uint8_t _numerator=4,uint8_t _denominator=4,uint8_t _tickPerMidiclock=24,uint8_t _num32ndNotePer24Midiclock=8);
                TimeSignature(const TimeSignature&)=default;
                ~TimeSignature(void)=default;
        };
        bool operator==(const TimeSignature&a,const TimeSignature&b);
        bool operator!=(const TimeSignature&a,const TimeSignature&b);
        bool operator<(const TimeSignature&a,const TimeSignature&b);
        bool operator>(const TimeSignature&a,const TimeSignature&b);
        bool operator<=(const TimeSignature&a,const TimeSignature&b);
        bool operator>=(const TimeSignature&a,const TimeSignature&b);



        using TsList=MidiEventList<TimeSignature>;


        using TsMap=MidiEventMap<TimeSignature>;

    }
}
#endif
