/********************************************************************************************************
 * File Name    : MidiFile.hpp
 * Author       : Csrua / Gold_RsC
 * github       : Gold-RsC(https://github.com/Gold-RsC)
 * bilibili     : Csrua(https://space.bilibili.com/361846321)
 * Email        : 310106329@qq.com
 * Create Date  : 2020/04/26
 * Update Date  : 2025/12/01
 * Details      : README.md 
 ********************************************************************************************************/
#ifndef MIDIFILE_HPP
#define MIDIFILE_HPP
#include"MidiHead.hpp"
#include"MidiTrack.hpp"
#include<string>

namespace GoldType{
    namespace MidiParse{
        class MidiFile:public MidiObject {
            protected:
                enum class MidiFileState:uint8_t{
                    untouched,
                    read_error,
                    read_success,
                    write_error,
                    write_success
                };
                MidiFileState m_state;
            public:
                std::string filename;
                MidiHead head;
                MidiTrackList tracks;
            protected:
                uint32_t read_staticData(FILE*fin,size_t num);
                struct __ReadDynamicData_t{
                    MidiErrorType type;
                    uint32_t data;
                    uint8_t length:7;
                    uint8_t is_error:1;
                    uint8_t buffer[4];
                };
                __ReadDynamicData_t read_dynamicData(FILE*fin);
            protected:
                MidiErrorType read_midiHead(FILE*fin);
                MidiErrorType read_midiTrack(FILE*fin);
                MidiErrorType read_midiMessage(FILE*fin,uint8_t&last_eventType,MidiEvent&event,uint32_t&byte_read);
                MidiErrorType _read_fin(FILE*fin);
            public:
                MidiFile(void)=delete;
                MidiFile(std::string _filename);
                MidiFile(const MidiFile&_midiFile);
                MidiFile(MidiFile&&_midiFile);
                ~MidiFile(void)=default;
            public:
                MidiErrorType read(void);
            public:
                MidiFile&operator=(const MidiFile&_midi);
                MidiFile&operator=(MidiFile&&_midi);
                MidiTrack&operator[](size_t idx);
                const MidiTrack&operator[](size_t idx) const;
            public:
                bool is_untouched(void)const;
                bool is_read_success(void)const;
                bool is_read_error(void)const;
                bool is_write_success(void)const;
                bool is_write_error(void)const;

                MidiErrorType get_error(MidiError&_midiError=midiError)const override final;
        };

        
    }
}

#endif
