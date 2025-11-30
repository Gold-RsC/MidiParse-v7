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
#ifndef NOTE_HPP
#define NOTE_HPP
#include"BasicMidiEvent.hpp"

namespace GoldType{
    namespace MidiParse{
        class Note:public BasicMidiEvent_Non{
            public:
                uint8_t pitch;
                uint8_t velocity;
                uint8_t instrument;
                double bar;
                double beat;
            public:
                Note(uint64_t _time=0,MidiTimeMode _timeMode=MidiTimeMode::tick,uint8_t _track=0,uint8_t _channel=0,uint8_t _pitch=0,uint8_t _velocity=0,uint8_t _instrument=0,double _bar=0,double _beat=0);
                Note(const Note&)=default;
                ~Note(void)=default;

                bool is_empty(void)const;
        };
        bool operator==(const Note&a,const Note&b);
        bool operator!=(const Note&a,const Note&b);
        bool operator<(const Note&a,const Note&b);
        bool operator>(const Note&a,const Note&b);
        bool operator<=(const Note&a,const Note&b);
        bool operator>=(const Note&a,const Note&b);
        
        using NoteList=MidiEventList<Note>;

        using NoteMap=MidiEventMap<Note>;
    }
}
#endif
