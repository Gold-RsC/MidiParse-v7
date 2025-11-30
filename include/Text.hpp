/********************************************************************************************************
 * File Name    : Text.hpp
 * Author       : Csrua / Gold_RsC
 * github       : Gold-RsC(https://github.com/Gold-RsC)
 * bilibili     : Csrua(https://space.bilibili.com/361846321)
 * Email        : 310106329@qq.com
 * Create Date  : 2020/04/26
 * Update Date  : 2025/12/01
 * Details      : README.md 
 ********************************************************************************************************/
#ifndef TEXT_HPP
#define TEXT_HPP

#include"BasicMidiEvent.hpp"
#include<string>
namespace GoldType{
    namespace MidiParse{
        class Text:public BasicMidiEvent_Meta{
            public:
                uint8_t type;
                std::string text;
            public:
                Text(uint64_t _time=0,MidiTimeMode _timeMode=MidiTimeMode::tick,uint8_t _track=0,uint8_t _type=0,const std::string&_text="");
                Text(const Text&)=default;
                ~Text(void)=default;
        };
        bool operator==(const Text&a,const Text&b);
        bool operator!=(const Text&a,const Text&b);
        bool operator<(const Text&a,const Text&b);
        bool operator>(const Text&a,const Text&b);
        bool operator<=(const Text&a,const Text&b);
        bool operator>=(const Text&a,const Text&b);

        using TextList=MidiEventList<Text>;

        using TextMap=MidiEventMap<Text>;
    }
}
#endif
