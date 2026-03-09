/********************************************************************************************************
 * File Name    : Tempo.hpp
 * Author       : Csrua / Gold_RsC
 * github       : Gold-RsC(https://github.com/Gold-RsC)
 * bilibili     : Csrua(https://space.bilibili.com/361846321)
 * Email        : 310106329@qq.com
 * Update Date  : 2026/03/06
 * Details      : README.md
 ********************************************************************************************************/
#ifndef TEMPO_HPP
#define TEMPO_HPP

#include "BasicMidiEvent.hpp"

namespace GoldType::MidiParse {


class Tempo : public BasicMidiEvent_Meta {
public:
    uint32_t mispqn;
    uint64_t timeNode;

public:
    Tempo(MidiTime _time = 0, MidiTimeMode _timeMode = MidiTimeMode::tick, MidiTrackNum _track = 0,
          uint32_t _mispqn = 0x07A120, uint64_t _timeNode = 0)
        : BasicMidiEvent_Meta(_time, _timeMode, _track),
          mispqn(_mispqn),
          timeNode(_timeNode) {
    }
    Tempo(const Tempo&) = default;
    ~Tempo(void) = default;

public:
    double bpm(uint8_t denominator) const {
        return 1.5e7 * denominator / mispqn;
    }
    MidiTime& tick(void) {
        if (timeMode == MidiTimeMode::tick) {
            return time;
        }
        return timeNode;
    }
    const MidiTime& tick(void) const {
        if (timeMode == MidiTimeMode::tick) {
            return time;
        }
        return timeNode;
    }
    MidiTime& microsecond(void) {
        if (timeMode == MidiTimeMode::microsecond) {
            return time;
        }
        return timeNode;
    }
    const MidiTime& microsecond(void) const {
        if (timeMode == MidiTimeMode::microsecond) {
            return time;
        }
        return timeNode;
    }

public:
    MidiErrorCode get_errorCode(void) const noexcept final {
        if (track & 0xF0) {
            return MidiErrorCode::event_track;
        }
        return MidiErrorCode::no_error;
    }
};
bool operator==(const Tempo& a, const Tempo& b) {
    return a.timeMode == b.timeMode && a.time == b.time && a.track == b.track && a.mispqn == b.mispqn;
}
bool operator!=(const Tempo& a, const Tempo& b) {
    return !(a == b);
}
bool operator<(const Tempo& a, const Tempo& b) {
    if (a.timeMode != b.timeMode) {
        return a.timeMode < b.timeMode;
    }
    if (a.time != b.time) {
        return a.time < b.time;
    }
    if (a.track != b.track) {
        return a.track < b.track;
    }
    return a.mispqn < b.mispqn;
}
bool operator>(const Tempo& a, const Tempo& b) {
    return b < a;
}
bool operator<=(const Tempo& a, const Tempo& b) {
    return !(a > b);
}
bool operator>=(const Tempo& a, const Tempo& b) {
    return !(a < b);
}

using TempoList = MidiEventList<Tempo>;

using TempoMap = MidiEventMap<Tempo>;

}  // namespace GoldType::MidiParse
#endif
