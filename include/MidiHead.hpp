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
                    throw_ignorably(MidiError(MidiErrorCode::head_division, 0xFF, 0xFFFFFFFF));
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

    MidiErrorCode get_errorCode(void) const noexcept final {
        if (!(format == 0 || format == 1 || format == 2)) {
            return MidiErrorCode::head_format;
        }
        if (format == 0 && ntracks != 1) {
            return MidiErrorCode::head_ntracks;
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
                    return MidiErrorCode::head_division;
                }
            }
        }
        return MidiErrorCode::no_error;
    }
    MidiError get_error(void) const noexcept final {
        MidiError err;
        err.code = this->get_errorCode();
        if (err.code != MidiErrorCode::no_error) {
            err.track_idx = -1;
            err.event_idx = -1;
        }
        return err;
    }
};

}  // namespace GoldType::MidiParse
#endif
