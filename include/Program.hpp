/********************************************************************************************************
 * File Name    : Program.hpp
 * Author       : Csrua / Gold_RsC
 * github       : Gold-RsC(https://github.com/Gold-RsC)
 * bilibili     : Csrua(https://space.bilibili.com/361846321)
 * Email        : 310106329@qq.com
 * Create Date  : 2020/04/26
 * Update Date  : 2025/12/01
 * Details      : README.md 
 ********************************************************************************************************/
#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include"BasicMidiEvent.hpp"

namespace GoldType{
    namespace MidiParse{
        class Program:public BasicMidiEvent_Non{
            public:
                uint8_t instrument;
            public:
                Program(uint64_t _time=0,MidiTimeMode _timeMode=MidiTimeMode::tick,uint8_t _track=0,uint8_t _channel=0,uint8_t _instrument=0);
                Program(const Program&)=default;
                ~Program(void)=default;
        };
        bool operator==(const Program&a,const Program&b);
        bool operator!=(const Program&a,const Program&b);
        bool operator<(const Program&a,const Program&b);
        bool operator>(const Program&a,const Program&b);
        bool operator<=(const Program&a,const Program&b);
        bool operator>=(const Program&a,const Program&b);

        using ProgramList=MidiEventList<Program>;

        using ProgramMap=MidiEventMap<Program>;

    }
}
#endif
