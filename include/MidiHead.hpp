/********************************************************************************************************
 * File Name    : MidiHead.hpp
 * Author       : Csrua / Gold_RsC
 * github       : Gold-RsC(https://github.com/Gold-RsC)
 * bilibili     : Csrua(https://space.bilibili.com/361846321)
 * Email        : 310106329@qq.com
 * Update Date  : 2026/03/06
 * Details      : README.md
 ********************************************************************************************************/
#ifndef MIDIHEAD_HPP
#define MIDIHEAD_HPP
#include "MidiError.hpp"
namespace GoldType::MidiParse {
class MidiHead : public MidiObject {
public:
    uint16_t format;
    uint16_t ntracks;
    uint16_t division;

public:
    MidiHead(uint16_t _format = 0, uint16_t _ntracks = 1, uint16_t _division = 120)
            : format(_format),
              ntracks(_ntracks),
              division(_division) {
    }
    uint16_t tpqn(void) const {
        // 15th
        // 1xxx xxxx xxxx xxxx
        if (division & 0x8000) {
            // 8th~14th
            // 1fff ffff xxxx xxxx
            uint16_t fps = 0x0100 - (division >> 8) & 0xFF;
            // 0th~7th
            // 1xxx xxxx tttt tttt
            uint16_t tpf = division & 0xFF;
            switch (fps) {
                case 24:
                    break;
                case 25:
                    break;
                case 29:
                    break;
                case 30:
                    break;
                default: {
                    midiError(MidiErrorType::head_division);
                    return 120;
                }
            }
            return fps * tpf;
        }
        // 0xxx xxxx xxxx xxxx
        else {
            return division;
        }
    }

    MidiErrorType get_error(MidiError& _midiError = midiError) const override final {
        if (!(format == 0 || format == 1 || format == 2)) {
            return _midiError(MidiErrorType::head_format);
        }
        if (format == 0 && ntracks != 1) {
            return _midiError(MidiErrorType::head_ntracks);
        }
        if (division & 0x8000) {
            uint16_t fps = 0x0100 - (division >> 8) & 0xFF;
            uint16_t tpf = division & 0xFF;
            switch (fps) {
                case 24:
                    break;
                case 25:
                    break;
                case 29:
                    break;
                case 30:
                    break;
                default: {
                    return _midiError(MidiErrorType::head_division);
                }
            }
        }
        return _midiError(MidiErrorType::no_error);
    }
};

}  // namespace GoldType::MidiParse
#endif
