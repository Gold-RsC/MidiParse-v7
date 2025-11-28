#ifndef MIDIHEAD_HPP
#define MIDIHEAD_HPP
#include"MidiError.hpp"
namespace GoldType{
    namespace MidiParse{
        /********************************
                    Midi Head
        ********************************/
        class MidiHead{
            public:
                uint16_t format;
                uint16_t ntracks;
                uint16_t division;
            public:
                MidiHead(uint16_t _format=0,uint16_t _ntracks=1,uint16_t _division=120):
                    format(_format),ntracks(_ntracks),division(_division){}
                uint16_t tpqn(void)const{
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
        };
        
        template<>
        MidiErrorType MidiError::operator()<MidiHead>(const MidiHead&_midiHead){
            if(!(_midiHead.format==0||_midiHead.format==1||_midiHead.format==2)) {
                return MidiErrorType::head_format;
            }
            if(_midiHead.format==0&&_midiHead.ntracks!=1) {
                return MidiErrorType::head_ntracks;
            }
            if(_midiHead.division&0x8000){
                uint16_t fps=0x0100-(_midiHead.division>>8)&0xFF;
                uint16_t tpf=_midiHead.division&0xFF;
                switch(fps){
                    case 24:break;
                    case 25:break;
                    case 29:break;
                    case 30:break;
                    default:{
                        return MidiErrorType::head_division;
                    }
                }
            }
            return MidiErrorType::no_error;
        }
    }
}
#endif
