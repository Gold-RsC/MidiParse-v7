#include"MidiMessage.hpp"
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
    }
}