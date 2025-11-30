#include"MidiEvent.hpp"

namespace GoldType{
    namespace MidiParse{
        MidiEventType MidiMessage::type(void)const {
            if(operator[](0)==0xFF||operator[](0)==0xF0||operator[](0)==0xF7){
                return MidiEventType(operator[](0));
            }
            else{
                return MidiEventType(operator[](0)&0xF0);
            }
        }
        uint8_t MidiMessage::channel(void)const {
#ifdef MIDI_DEBUG
            if(operator[](0)<0xF0&&operator[](0)>0x7F){
#endif
                return operator[](0)&0x0F;
#ifdef MIDI_DEBUG
            }
            return 0xFF;
#endif
        }
        MidiErrorType MidiMessage::get_error(MidiError&_midiError)const{
            MidiEventType _type=type();
            switch(_type){
                case MidiEventType::note_off:
                case MidiEventType::note_on:
                case MidiEventType::key_afterTouch:
                case MidiEventType::controller:
                case MidiEventType::pitchWheel:{
                    if(operator[](1)&0x80){
                        return _midiError(MidiErrorType((uint8_t)_type|0x01));
                    }
                    if(operator[](2)&0x80){
                        return _midiError(MidiErrorType((uint8_t)_type|0x02));
                    }
                    break;
                }
                case MidiEventType::program:
                case MidiEventType::channel_afterTouch:{
                    if(operator[](1)&0x80){
                        return _midiError(MidiErrorType((uint8_t)_type|0x01));
                    }
                    break;
                }
                case MidiEventType::sysex_begin:
                case MidiEventType::sysex_end:{
                    size_t i=1;
                    uint32_t length=0;
                    for(;i<5;++i) {
                        length=(length<<7)|(operator[](i)&0x7F);
                        if(operator[](i)<0x80) {
                            break;
                        }
                    }
                    if(i+length+1!=size()){
                        return _midiError(MidiErrorType::sysex_length);
                    }
                    break;
                }
                case MidiEventType::meta:{
                    size_t i=2;
                    uint32_t length=0;
                    for(;i<5;++i) {
                        length=(length<<7)|(operator[](i)&0x7F);
                        if(operator[](i)<0x80) {
                            break;
                        }
                    }
                    if(i+length+1!=size()){
                        return _midiError(MidiErrorType::meta_length);
                    }
                    break;
                }
                default:{
                    return _midiError(MidiErrorType::event_unknown_type);
                }
            }
            return _midiError(MidiErrorType::no_error);
        }
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
        MidiErrorType MidiEvent::get_error(MidiError&_midiError)const{
            return message.get_error(_midiError);
        }
        uint8_t&MidiEvent::operator[](size_t idx) {
            return message[idx];
        }
        const uint8_t&MidiEvent::operator[](size_t idx) const{
            return message[idx];
        }
    }
}