/********************************************************************************************************
 * File Name    : MidiEvent.hpp
 * Author       : Csrua / Gold_RsC
 * github       : Gold-RsC(https://github.com/Gold-RsC)
 * bilibili     : Csrua(https://space.bilibili.com/361846321)
 * Email        : 310106329@qq.com
 * Create Date  : 2020/04/26
 * Update Date  : 2025/12/01
 * Details      : README.md 
 ********************************************************************************************************/
#ifndef MIDIEVENT_HPP
#define MIDIEVENT_HPP
#include"BasicMidiEvent.hpp"
#include<algorithm>
namespace GoldType{
    namespace MidiParse{
        enum class MidiEventType:uint8_t{
            null=0x00,
            /******************
            Part1:type = 0xNx (N = 8~E ,x = 0~F)
            type & 0xF0 = 0xN0
            ******************/
            note_off=0x80,              // 2
            note_on=0x90,               // 2
            key_afterTouch=0xA0,        // 2
            controller=0xB0,            // 2
            program=0xC0,               // 1
            channel_afterTouch=0xD0,    // 1
            pitchWheel=0xE0,            // 2
            
            /*******************
            Part2:type = 0xF0 or 0xF7
            *******************/
            sysex_begin=0xF0,sysex_end=0xF7,
            
            /*******************
            Part3:type = 0xFF
            *******************/
            meta=0xFF
        };
        class MidiMessage:public std::vector<uint8_t>,public MidiObject{
            public:
                using std::vector<uint8_t>::vector;
                MidiEventType type(void)const;
                uint8_t channel(void)const;
                MidiErrorType get_error(MidiError&_midiError=midiError)const override final;
        };

        class MidiEvent:public BasicMidiEvent {
            public:
                MidiMessage message;
            public:
                MidiEvent(void);
                MidiEvent(const MidiEvent&another)=default;
                MidiEvent(const MidiMessage&_message);
                ~MidiEvent(void)=default;
            public:
                MidiEventType type(void)const;
                uint8_t channel(void)const;
            public:
                bool is_normal(void)const;
                bool is_meta(void)const;
                bool is_sysex(void)const;
                MidiErrorType get_error(MidiError&_midiError=midiError)const override final;
            public:
                uint8_t&operator[](size_t idx);
                const uint8_t&operator[](size_t idx)const;
                MidiEvent&operator=(const MidiEvent&another)=default;
        };
    }
}
#endif
