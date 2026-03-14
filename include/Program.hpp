/********************************************************************************************************
 * File Name    : Program.hpp
 * Author       : Csrua / Gold_RsC
 * github       : Gold-RsC(https://github.com/Gold-RsC)
 * bilibili     : Csrua(https://space.bilibili.com/361846321)
 * Email        : 310106329@qq.com
 * Update Date  : 2026/03/06
 * Details      : README.md
 ********************************************************************************************************/
#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include "BasicMidiEvent.hpp"

namespace GoldType::MidiParse {

class Program : public BasicMidiEvent_Non {
public:
    uint8_t instrument;

public:
    Program(MidiTime _time = 0, MidiTimeMode _timeMode = MidiTimeMode::tick, MidiTrackNum _track = 0,
            MidiChannelNum _channel = 0, uint8_t _instrument = 0)
        : BasicMidiEvent_Non(_time, _timeMode, _track, _channel),
          instrument(_instrument) {
    }
    Program(const Program&) = default;
    ~Program(void) = default;

public:
    MidiErrorCode get_errorCode(void) const noexcept final {
        if (track & 0xF0) {
            return MidiErrorCode::event_track;
        }
        if (channel > 0x0F) {
            return MidiErrorCode::event_channel;
        }
        if (instrument & 0xF0) {
            return MidiErrorCode::program_number;
        }
        return MidiErrorCode::no_error;
    }
    MidiError get_error(void) const noexcept final {
        MidiError err;
        err.code = get_errorCode();
        err.track_idx = track;
        err.event_idx = -1;
        return err;
    }
};
bool operator==(const Program& a, const Program& b) {
    return a.time == b.time && a.timeMode == b.timeMode && a.channel == b.channel && a.track == b.track &&
           a.instrument == b.instrument;
}
bool operator!=(const Program& a, const Program& b) {
    return !(a == b);
}
bool operator<(const Program& a, const Program& b) {
    if (a.timeMode != b.timeMode) {
        return a.timeMode < b.timeMode;
    }
    if (a.time != b.time) {
        return a.time < b.time;
    }
    if (a.channel != b.channel) {
        return a.channel < b.channel;
    }
    if (a.track != b.track) {
        return a.track < b.track;
    }
    return a.instrument < b.instrument;
}
bool operator>(const Program& a, const Program& b) {
    return b < a;
}
bool operator<=(const Program& a, const Program& b) {
    return !(a > b);
}
bool operator>=(const Program& a, const Program& b) {
    return !(a < b);
}

using ProgramList = MidiEventList<Program>;

using ProgramMap = MidiEventMap<Program>;


}  // namespace GoldType::MidiParse
#endif
