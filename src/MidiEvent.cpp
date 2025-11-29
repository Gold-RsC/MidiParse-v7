#include"MidiEvent.hpp"

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
        uint64_t BasicMidiEvent::__time_error_v=uint64_t(-1);
        
        BasicMidiEvent_Non::BasicMidiEvent_Non(uint64_t _time,MidiTimeMode _timeMode,uint8_t _track,uint8_t _channel):
            BasicMidiEvent(_time,_timeMode,_track),channel(_channel){}
        BasicMidiEvent_Meta::BasicMidiEvent_Meta(uint64_t _time,MidiTimeMode _timeMode,uint8_t _track):
            BasicMidiEvent(_time,_timeMode,_track){}
        
        MidiEvent::MidiEvent(void):
            message() {}
        MidiEvent::MidiEvent(const MidiMessage&_message):
            message(_message) {}
        MidiEventType MidiEvent::type(void)const {
            return message.type();
        }
        uint8_t MidiEvent::channel(void)const {
            return message.channel();
        }
        bool MidiEvent::is_normal(void)const{
            return !(is_meta()||is_sysex());
        }
        bool MidiEvent::is_meta(void)const {
            return type()==MidiEventType::meta;
        }
        bool MidiEvent::is_sysex(void)const{
            return type()==MidiEventType::sysex_begin||type()==MidiEventType::sysex_end;
        }
        uint8_t&MidiEvent::operator[](size_t idx) {
            return message[idx];
        }
        const uint8_t&MidiEvent::operator[](size_t idx) const{
            return message[idx];
        }

        // MidiErrorType get_error(MidiError&_midiError,const MidiEvent&_midiEvent){
        //     MidiErrorType type=MidiErrorType::no_error;
        //     switch(_midiEvent.type()){
        //         case MidiEventType::note_off:
        //         case MidiEventType::note_on:
        //         case MidiEventType::key_afterTouch:
        //         case MidiEventType::controller:
        //         case MidiEventType::pitchWheel:{
        //             if(_midiEvent[1]&0x80){
        //                 return _midiError(MidiErrorType((uint8_t)type|0x01));
        //             }
        //             if(_midiEvent[2]&0x80){
        //                 return _midiError(MidiErrorType((uint8_t)type|0x02));
        //             }
        //             break;
        //         }
        //         case MidiEventType::program:
        //         case MidiEventType::channel_afterTouch:{
        //             if(_midiEvent[1]&0x80){
        //                 return _midiError(MidiErrorType((uint8_t)type|0x01));
        //             }
        //             break;
        //         }
        //         case MidiEventType::sysex_begin:
        //         case MidiEventType::sysex_end:{
        //             size_t i=1;
        //             uint32_t length=0;
        //             for(;i<5;++i) {
        //                 length=(length<<7)|(_midiEvent[i]&0x7F);
        //                 if(_midiEvent[i]<0x80) {
        //                     break;
        //                 }
        //             }
        //             if(i+length+1!=_midiEvent.message.size()){
        //                 return _midiError(MidiErrorType::sysex_length);
        //             }
        //             break;
        //         }
        //         case MidiEventType::meta:{
        //             size_t i=2;
        //             uint32_t length=0;
        //             for(;i<5;++i) {
        //                 length=(length<<7)|(_midiEvent[i]&0x7F);
        //                 if(_midiEvent[i]<0x80) {
        //                     break;
        //                 }
        //             }
        //             if(i+length+1!=_midiEvent.message.size()){
        //                 return _midiError(MidiErrorType::meta_length);
        //             }
        //             break;
        //         }
        //         default:{
        //             return _midiError(MidiErrorType::event_unknown_type);
        //         }
        //     }
        //     return _midiError(MidiErrorType::no_error);
        // }
    }
}