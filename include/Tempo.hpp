/********************************************************************************************************
 * File Name    : MoreMidiEvent.hpp
 * Author       : Csrua / Gold_RsC
 * github       : Gold-RsC(https://github.com/Gold-RsC)
 * bilibili     : Csrua(https://space.bilibili.com/361846321)
 * QQ           : 310106329
 * Email        : 310106329@qq.com
 * Create Date  : 2020/07/26
 * Update Date  : 2025/12/01
 * Details      : README.md 
 ********************************************************************************************************/
#ifndef TEMPO_HPP
#define TEMPO_HPP

#include"BasicMidiEvent.hpp"

namespace GoldType{
    namespace MidiParse{


        class Tempo:public BasicMidiEvent_Meta{
            public:
                uint32_t mispqn;
                uint64_t timeNode;
            public:
                Tempo(uint64_t _time=0,MidiTimeMode _timeMode=MidiTimeMode::tick,uint8_t _track=0,uint32_t _mispqn=0x07A120,uint64_t _timeNode=0);
                Tempo(const Tempo&)=default;
                ~Tempo(void)=default;
            public:
                double bpm(void)const;
                uint64_t&tick(void);
                const uint64_t&tick(void)const;
                uint64_t&microsecond(void);
                const uint64_t&microsecond(void)const;
        };
        bool operator==(const Tempo&a,const Tempo&b);
        bool operator!=(const Tempo&a,const Tempo&b);
        bool operator<(const Tempo&a,const Tempo&b);
        bool operator>(const Tempo&a,const Tempo&b);
        bool operator<=(const Tempo&a,const Tempo&b);
        bool operator>=(const Tempo&a,const Tempo&b);

        using TempoList=MidiEventList<Tempo>;

        using TempoMap=MidiEventMap<Tempo>;
    }
}
#endif
