#include"MidiError.hpp"
namespace GoldType{
    namespace MidiParse{
        MidiError::MidiError(void):
            m_file(stderr){}
        MidiError::MidiError(const char*_name):
            m_file(fopen(_name,"w")){}
        MidiError::~MidiError(void){
            if(m_file!=stderr){
                fclose(m_file);
            }
            m_file=nullptr;
        }
        void MidiError::replace(const char*_name){
            this->~MidiError();
            m_file=fopen(_name,"w");
        }
        void MidiError::write_type(MidiErrorType _mErrType){
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
        void MidiError::write_text(const char*_text){
            fprintf(m_file,"MidiError:\n\t");
            fprintf(m_file,_text);
        }
        MidiError&MidiError::operator<<(MidiErrorType _mErrType){
            write_type(_mErrType);
            return *this;
        }
        MidiError&MidiError::operator<<(const char*_text){
            write_text(_text);
            return *this;
        }
        MidiErrorType MidiError::operator()(MidiErrorType _mErrType){
#ifdef MIDI_DEBUG
            write_type(_mErrType);
#endif
            return _mErrType;
        }
        MidiError midiError;

    }
}