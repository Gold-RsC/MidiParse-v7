#include"BasicMidiEvent.hpp"
namespace GoldType{
    namespace MidiParse{
        BasicMidiEvent::BasicMidiEvent(uint64_t _time,MidiTimeMode _timeMode,uint8_t _track):
            time(_time),timeMode(_timeMode),track(_track){}
        uint64_t&BasicMidiEvent::tick(void){
            if(timeMode==MidiTimeMode::tick){
                return time;
            }
            return __time_error_v;
        }
        const uint64_t&BasicMidiEvent::tick(void)const {
            if(timeMode==MidiTimeMode::tick){
                return time;
            }
            return __time_error_v;
        }
        uint64_t&BasicMidiEvent::microsecond(void){
            if(timeMode==MidiTimeMode::microsecond){
                return time;
            }
            return __time_error_v;
        }
        const uint64_t&BasicMidiEvent::microsecond(void)const {
            if(timeMode==MidiTimeMode::microsecond){
                return time;
            }
            return __time_error_v;
        }
        MidiErrorType BasicMidiEvent::get_error(MidiError&_midiError)const {
            return MidiErrorType::no_error;
        }

        uint64_t BasicMidiEvent::__time_error_v=uint64_t(-1);
        
        BasicMidiEvent_Non::BasicMidiEvent_Non(uint64_t _time,MidiTimeMode _timeMode,uint8_t _track,uint8_t _channel):
            BasicMidiEvent(_time,_timeMode,_track),channel(_channel){}
        MidiErrorType BasicMidiEvent_Non::get_error(MidiError&_midiError)const{
            return MidiErrorType::no_error;
        }
        BasicMidiEvent_Meta::BasicMidiEvent_Meta(uint64_t _time,MidiTimeMode _timeMode,uint8_t _track):
            BasicMidiEvent(_time,_timeMode,_track){}
        MidiErrorType BasicMidiEvent_Meta::get_error(MidiError&_midiError)const{
            return MidiErrorType::no_error;
        }
    }
}
