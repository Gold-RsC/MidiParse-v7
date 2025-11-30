/********************************************************************************************************
 * File Name    : NotePair.hpp
 * Author       : Csrua / Gold_RsC
 * github       : Gold-RsC(https://github.com/Gold-RsC)
 * bilibili     : Csrua(https://space.bilibili.com/361846321)
 * Email        : 310106329@qq.com
 * Create Date  : 2020/04/26
 * Update Date  : 2025/12/01
 * Details      : README.md 
 ********************************************************************************************************/
#ifndef NOTEPAIR_HPP
#define NOTEPAIR_HPP
#include"Note.hpp"
namespace GoldType{
    namespace MidiParse{
        bool is_notePair(const Note&_begin,const Note&_end);
        class NotePair:public Note{
            public:
                uint64_t duration;
                double bar_diff;
                double beat_diff;
            public:
                NotePair(uint64_t _time=0,uint64_t _duration=0,MidiTimeMode _timeMode=MidiTimeMode::tick,uint8_t _track=0,uint8_t _channel=0,uint8_t _pitch=0,uint8_t _velocity=0,uint8_t _instrument=0,double _bar=0,double _bar_diff=0,double _beat=0,double _beat_diff=0);
                NotePair(const Note&_begin,const Note&_end);
                NotePair(const NotePair&)=default;
                ~NotePair(void)=default;
            public:
                std::pair<Note,Note> devide(void)const;
        };
        bool operator==(const NotePair&a,const NotePair&b);
        bool operator!=(const NotePair&a,const NotePair&b);
        bool operator<(const NotePair&a,const NotePair&b);
        bool operator>(const NotePair&a,const NotePair&b);
        bool operator<=(const NotePair&a,const NotePair&b);
        bool operator>=(const NotePair&a,const NotePair&b);

        using NotePairList=MidiEventList<NotePair>;
        using NotePairMap=MidiEventMap<NotePair>;

        NotePairList link_notePair(const NoteList&_noteList);
        NotePairMap link_notePair(const NoteMap&_noteMap);
        NoteList devide_notePair(const NotePairList&_list);
        NoteMap devide_notePair(const NotePairMap&_map);
    }
}
#endif