#include"MidiHead.hpp"
namespace GoldType{
    namespace MidiParse{
        MidiHead::MidiHead(uint16_t _format,uint16_t _ntracks,uint16_t _division):
            format(_format),ntracks(_ntracks),division(_division){}
        uint16_t MidiHead::tpqn(void)const{
            //15th
            //1xxx xxxx xxxx xxxx
            if(division&0x8000){
                //8th~14th
                //1fff ffff xxxx xxxx
                uint16_t fps=0x0100-(division>>8)&0xFF;
                //0th~7th
                //1xxx xxxx tttt tttt
                uint16_t tpf=division&0xFF;
                switch(fps){
                    case 24:break;
                    case 25:break;
                    case 29:break;
                    case 30:break;
                    default:{
                        midiError(MidiErrorType::head_division);
                        return 120;
                    }
                }
                return fps*tpf;
            }
            //0xxx xxxx xxxx xxxx
            else{
                return division;
            }
        }
        
        MidiErrorType MidiHead::get_error(MidiError&_midiError)const{
            if(!(format==0||format==1||format==2)) {
                return _midiError(MidiErrorType::head_format);
            }
            if(format==0&&ntracks!=1) {
                return _midiError(MidiErrorType::head_ntracks);
            }
            if(division&0x8000){
                uint16_t fps=0x0100-(division>>8)&0xFF;
                uint16_t tpf=division&0xFF;
                switch(fps){
                    case 24:break;
                    case 25:break;
                    case 29:break;
                    case 30:break;
                    default:{
                        return _midiError(MidiErrorType::head_division);
                    }
                }
            }
            return _midiError(MidiErrorType::no_error);
        }
    }
}