#ifndef MIDIERROR_HPP
#define MIDIERROR_HPP
// #define MIDI_DEBUG
#include<stdio.h>
#include<stdint.h>

namespace GoldType{
    namespace MidiParse{
        #define MIDI_DEBUG_PRINT(...) midiDebug<<__VA_ARGS__
        #define MIDI_DEBUG_CODE(code) do{code}while(0)
        /********************************
                    Midi Error
        ********************************/
        
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
            public:
                MidiError(void):
                    m_file(stderr){}
                MidiError(const char*_name):
                    m_file(fopen(_name,"w")){}
                ~MidiError(void){
                    if(m_file!=stderr){
                        fclose(m_file);
                    }
                    m_file=nullptr;
                }
            public:
                void replace(const char*_name){
                    this->~MidiError();
                    m_file=fopen(_name,"w");
                }
            private:
                void write_type(MidiErrorType _mErrType){
                    if(_mErrType==MidiErrorType::no_error){
                        return;
                    }
                    fprintf(m_file,"MidiError:\n\t");
                    switch(_mErrType){
                        case MidiErrorType::filename:{
                            fprintf(m_file,"There is no file named this!\n");
                            break;
                        }
                        case MidiErrorType::head_identification:{
                            fprintf(m_file,"An error occurred in the head trunk!\n\t");
                            fprintf(m_file,"Head trunk identification error!\n");
                            break;
                        }
                        case MidiErrorType::head_length:{
                            fprintf(m_file,"An error occurred in the head trunk!\n\t");
                            fprintf(m_file,"Head trunk length error!\n");
                            break;
                        }
                        case MidiErrorType::head_format:{
                            fprintf(m_file,"An error occurred in the head trunk!\n\t");
                            fprintf(m_file,"Head trunk format error!\n");
                            break;
                        }
                        case MidiErrorType::head_ntracks:{
                            fprintf(m_file,"An error occurred in the head trunk!\n\t");
                            fprintf(m_file,"Head trunk track count error!\n");
                            break;
                        }
                        case MidiErrorType::head_division:{
                            fprintf(m_file,"An error occurred in the head trunk!\n\t");
                            fprintf(m_file,"Head trunk division error!\n");
                            break;
                        }
                        case MidiErrorType::track_identification:{
                            fprintf(m_file,"An error occurred in a track trunk!\n\t");
                            fprintf(m_file,"Track trunk identification error!\n");
                            break;
                        }
                        case MidiErrorType::track_length:{
                            fprintf(m_file,"An error occurred in a track trunk!\n\t");
                            fprintf(m_file,"Track trunk length error!\n");
                            break;
                        }
                        case MidiErrorType::event_deltaTime:{
                            fprintf(m_file,"An error occurred in an event!\n\t");
                            fprintf(m_file,"Event delta time error!\n");
                            break;
                        }
                        case MidiErrorType::event_unknown_type:{
                            fprintf(m_file,"An error occurred in an event!\n\t");
                            fprintf(m_file,"Event type error!\n");
                            break;
                        }
                        case MidiErrorType::noteOff_pitch:{
                            fprintf(m_file,"An error occurred in a note_off event!\n\t");
                            fprintf(m_file,"Pitch error!\n");
                            break;
                        }
                        case MidiErrorType::noteOff_velocity:{
                            fprintf(m_file,"An error occurred in a note_off event!\n\t");
                            fprintf(m_file,"Velocity error!\n");
                            break;
                        }
                        case MidiErrorType::noteOn_pitch:{
                            fprintf(m_file,"An error occurred in a note_on event!\n\t");
                            fprintf(m_file,"Pitch error!\n");
                            break;
                        }
                        case MidiErrorType::noteOn_velocity:{
                            fprintf(m_file,"An error occurred in a note_on event!\n\t");
                            fprintf(m_file,"Velocity error!\n");
                            break;
                        }
                        case MidiErrorType::keyAfterTouch_pitch:{
                            fprintf(m_file,"An error occurred in a key_after_touch event!\n\t");
                            fprintf(m_file,"Pitch error!\n");
                            break;
                        }
                        case MidiErrorType::keyAfterTouch_velocity:{
                            fprintf(m_file,"An error occurred in a key_after_touch event!\n\t");
                            fprintf(m_file,"Velocity error!\n");
                            break;
                        }
                        case MidiErrorType::controller_number:{
                            fprintf(m_file,"An error occurred in a controller event!\n\t");
                            fprintf(m_file,"Number error!\n");
                            break;
                        }
                        case MidiErrorType::controller_value:{
                            fprintf(m_file,"An error occurred in a controller event!\n\t");
                            fprintf(m_file,"Value error!\n");
                            break;
                        }
                        case MidiErrorType::program_number:{
                            fprintf(m_file,"An error occurred in a program event!\n\t");
                            fprintf(m_file,"Number error!\n");
                            break;
                        }
                        case MidiErrorType::channelAfterTouch_velocity:{
                            fprintf(m_file,"An error occurred in a channel_after_touch event!\n\t");
                            fprintf(m_file,"Velocity error!\n");
                            break;
                        }
                        case MidiErrorType::pitchWheel_mod:{
                            fprintf(m_file,"An error occurred in a pitchWheel event!\n\t");
                            fprintf(m_file,"Mod number error!\n");
                            break;
                        }
                        case MidiErrorType::pitchWheel_div:{
                            fprintf(m_file,"An error occurred in a pitchWheel event!\n\t");
                            fprintf(m_file,"Div number error!\n");
                            break;
                        }
                        case MidiErrorType::sysex_length:{
                            fprintf(m_file,"An error occurred in a sysex event!\n\t");
                            fprintf(m_file,"Length error!\n");
                            break;
                        }
                        case MidiErrorType::sysex_data:{
                            fprintf(m_file,"An error occurred in a sysex event!\n\t");
                            fprintf(m_file,"Data error!\n");
                            break;
                        }
                        case MidiErrorType::meta_length:{
                            fprintf(m_file,"An error occurred in a meta event!\n\t");
                            fprintf(m_file,"Length error!\n");
                            break;
                        }
                        case MidiErrorType::meta_data:{
                            fprintf(m_file,"An error occurred in a meta event!\n\t");
                            fprintf(m_file,"Data error!\n");
                            break;
                        }
                        default:{
                            fprintf(m_file,"Unknown error!\n");
                            break;
                        }
                    }
                }
                void write_text(const char*_text){
                    fprintf(m_file,"MidiError:\n\t");
                    fprintf(m_file,_text);
                }
            public:
                MidiError&operator<<(MidiErrorType _mErrType){
                    write_type(_mErrType);
                    return *this;
                }
                MidiError&operator<<(const char*_text){
                    write_text(_text);
                    return *this;
                }
                MidiErrorType operator()(MidiErrorType _mErrType){
#ifdef MIDI_DEBUG
                    write_type(_mErrType);
#endif
                    return _mErrType;
                }
                template<typename T>
                MidiErrorType operator()(const T&_t){
                    return MidiErrorType::unknown_error;
                }
        };
        MidiError midiError;

    }
}
#endif
