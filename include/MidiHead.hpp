/********************************************************************************************************
 * File Name    : MidiHead.hpp
 * Author       : Csrua / Gold_RsC
 * github       : https://github.com/Gold-RsC
 * bilibili     : https://space.bilibili.com/361846321
 * QQ           : 310106329
 * Email        : 310106329@qq.com
 * Create Date  : 2020/07/26
 * Update Date  : 2025/12/01
 * Details      : README.md 
 ********************************************************************************************************/
#ifndef MIDIHEAD_HPP
#define MIDIHEAD_HPP
#include"MidiError.hpp"
namespace GoldType{
    namespace MidiParse{
        class MidiHead{
            public:
                uint16_t format;
                uint16_t ntracks;
                uint16_t division;
            public:
                MidiHead(uint16_t _format=0,uint16_t _ntracks=1,uint16_t _division=120);
                uint16_t tpqn(void)const;
        };
        
    }
}
#endif
