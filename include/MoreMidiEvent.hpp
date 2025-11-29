/********************************************************************************************************
 * File Name    : MoreMidiEvent.hpp
 * Author       : Csrua / Gold_RsC
 * github       : https://github.com/Gold-RsC
 * bilibili     : https://space.bilibili.com/361846321
 * QQ           : 310106329
 * Email        : 310106329@qq.com
 * Create Date  : 2020/07/26
 * Update Date  : 2025/12/01
 * Details      : README.md 
 ********************************************************************************************************/
#ifndef MOREMIDIEVENT_HPP
#define MOREMIDIEVENT_HPP
#include"MidiEvent.hpp"
#include<string>
#include<map>
#include<queue>
namespace GoldType{
    namespace MidiParse{
        /********************************
                    More Midi Event
        ********************************/
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
        
        
        using NoteList=MidiEventList<Note>;
        using TextList=MidiEventList<Text>;
        using TempoList=MidiEventList<Tempo>;
        using TsList=MidiEventList<TimeSignature>;
        using ProgramList=MidiEventList<Program>;
        using BarBeatList=MidiEventList<BarBeat>;

        using NoteMap=MidiEventMap<Note>;
        using TextMap=MidiEventMap<Text>;
        using TempoMap=MidiEventMap<Tempo>;
        using TsMap=MidiEventMap<TimeSignature>;
        using ProgramMap=MidiEventMap<Program>;
        using BarBeatMap=MidiEventMap<BarBeat>;
    }
}
#endif
