/********************************************************************************************************
 * File Name    : MidiError.hpp
 * Author       : Csrua / Gold_RsC
 * github       : https://github.com/Gold-RsC
 * bilibili     : https://space.bilibili.com/361846321
 * QQ           : 310106329
 * Email        : 310106329@qq.com
 * Create Date  : 2020/07/26
 * Update Date  : 2025/12/01
 * Details      : README.md 
 ********************************************************************************************************/
#ifndef MIDIERROR_HPP
#define MIDIERROR_HPP
#include<stdio.h>
#include<stdint.h>

namespace GoldType{
    namespace MidiParse{
        enum class MidiErrorType:uint8_t{
            no_error=0x00,
            filename=0x01,
            head_identification=0x02,
            head_length=0x03,
            head_format=0x04,
            head_ntracks=0x05,
            head_division=0x06,
            track_identification=0x07,
            track_length=0x08,
            event_deltaTime=0x09,
            event_unknown_type=0x0A,
            
            noteOff_pitch=0x81,
            noteOff_velocity=0x82,
            noteOn_pitch=0x91,
            noteOn_velocity=0x92,
            keyAfterTouch_pitch=0xA1,
            keyAfterTouch_velocity=0xA2,
            controller_number=0xB1,
            controller_value=0xB2,
            program_number=0xC1,
            channelAfterTouch_velocity=0xD1,
            pitchWheel_mod=0xE1,
            pitchWheel_div=0xE2,
            sysex_length=0xF0,
            sysex_data=0xF1,
            meta_length=0xF8,
            meta_data=0xF9,
            parse_error=0x10,
            change_timeMode=0x11,

            unknown_error=0x12,
        };

        
        class MidiError{
            protected:
                FILE*m_file;
            private:
                void write_type(MidiErrorType _mErrType);
                void write_text(const char*_text);
            public:
                MidiError(void);
                MidiError(const char*_name);
                ~MidiError(void);

                void replace(const char*_name);

                MidiError&operator<<(MidiErrorType _mErrType);
                MidiError&operator<<(const char*_text);

                MidiErrorType operator()(MidiErrorType _mErrType);
                // template<typename T>
                // MidiErrorType operator()(const T&_t){
                //     return get_error(*this,_t);
                // }
        };
        extern MidiError midiError;

        // class MidiEvent;
        // class MidiTrack;
        // class MidiTrackList;
        // class MidiHead;
        // class MidiFile;
        // MidiErrorType get_error(MidiError&_midiError,const MidiEvent&event);
        // MidiErrorType get_error(MidiError&_midiError,const MidiTrack&track);
        // MidiErrorType get_error(MidiError&_midiError,const MidiTrackList&tracks);
        // MidiErrorType get_error(MidiError&_midiError,const MidiHead&head);
        // MidiErrorType get_error(MidiError&_midiError,const MidiFile&file);
    }
}
#endif
