#include"BasicMidiEvent.hpp"
namespace GoldType{
    namespace MidiParse{
        BasicMidiEvent::BasicMidiEvent(MidiTime _time,MidiTimeMode _timeMode,MidiTrackNum _track):
            time(_time),timeMode(_timeMode),track(_track){}
        MidiTime&BasicMidiEvent::tick(void){
            if(timeMode==MidiTimeMode::tick){
                return time;
            }
            return __time_error_v;
        }
        const MidiTime&BasicMidiEvent::tick(void)const {
            if(timeMode==MidiTimeMode::tick){
                return time;
            }
            return __time_error_v;
        }
        MidiTime&BasicMidiEvent::microsecond(void){
            if(timeMode==MidiTimeMode::microsecond){
                return time;
            }
            return __time_error_v;
        }
        const MidiTime&BasicMidiEvent::microsecond(void)const {
            if(timeMode==MidiTimeMode::microsecond){
                return time;
            }
            return __time_error_v;
        }
        

        MidiTime BasicMidiEvent::__time_error_v=MidiTime(-1);
        
        BasicMidiEvent_Non::BasicMidiEvent_Non(MidiTime _time,MidiTimeMode _timeMode,MidiTrackNum _track,MidiChannelNum _channel):
            BasicMidiEvent(_time,_timeMode,_track),channel(_channel){}
        
        BasicMidiEvent_Meta::BasicMidiEvent_Meta(MidiTime _time,MidiTimeMode _timeMode,MidiTrackNum _track):
            BasicMidiEvent(_time,_timeMode,_track){}
        
    }
}
