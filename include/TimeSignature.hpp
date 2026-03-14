/********************************************************************************************************
 * File Name    : TimeSignature.hpp
 * Author       : Csrua / Gold_RsC
 * github       : Gold-RsC(https://github.com/Gold-RsC)
 * bilibili     : Csrua(https://space.bilibili.com/361846321)
 * Email        : 310106329@qq.com
 * Update Date  : 2026/03/06
 * Details      : README.md
 ********************************************************************************************************/
#ifndef TIMESIGNATURE_HPP
#define TIMESIGNATURE_HPP

#include "BasicMidiEvent.hpp"
#include <map>
#include <queue>
#include <string>

namespace GoldType::MidiParse {


class TimeSignature : public BasicMidiEvent_Meta {
public:
    // beat / bar
    uint8_t numerator;
    // beat / qn
    uint8_t denominator;
    uint8_t tickPerMidiclock;
    uint8_t num32ndNotePer24Midiclock;

public:
    TimeSignature(MidiTime _time = 0, MidiTimeMode _timeMode = MidiTimeMode::tick, MidiTrackNum _track = 0,
                  uint8_t _numerator = 4, uint8_t _denominator = 4, uint8_t _tickPerMidiclock = 24,
                  uint8_t _num32ndNotePer24Midiclock = 8)
        : BasicMidiEvent_Meta(_time, _timeMode, _track),
          numerator(_numerator),
          denominator(_denominator),
          tickPerMidiclock(_tickPerMidiclock),
          num32ndNotePer24Midiclock(_num32ndNotePer24Midiclock) {
    }
    TimeSignature(const TimeSignature&) = default;
    ~TimeSignature(void) = default;

public:
    MidiErrorCode get_errorCode(void) const noexcept override {
        if (track & 0xF0) {
            return MidiErrorCode::event_track;
        }
        if ((denominator & (denominator - 1)) == 0 && tickPerMidiclock == 24 && num32ndNotePer24Midiclock == 8) {
            return MidiErrorCode::no_error;
        }
        return MidiErrorCode::meta_data;
    }
    MidiError get_error(void) const noexcept override {
        MidiError err;
        err.code = get_errorCode();
        err.track_idx = track;
        err.event_idx = -1;
        return err;
    }
};
bool operator==(const TimeSignature& a, const TimeSignature& b) {
    return a.time == b.time && a.timeMode == b.timeMode && a.track == b.track && a.numerator == b.numerator &&
           a.denominator == b.denominator && a.tickPerMidiclock == b.tickPerMidiclock &&
           a.num32ndNotePer24Midiclock == b.num32ndNotePer24Midiclock;
}
bool operator!=(const TimeSignature& a, const TimeSignature& b) {
    return !(a == b);
}
bool operator<(const TimeSignature& a, const TimeSignature& b) {
    if (a.timeMode != b.timeMode) {
        return a.timeMode < b.timeMode;
    }
    if (a.time != b.time) {
        return a.time < b.time;
    }
    if (a.track != b.track) {
        return a.track < b.track;
    }
    if (a.numerator != b.numerator) {
        return a.numerator < b.numerator;
    }
    if (a.denominator != b.denominator) {
        return a.denominator < b.denominator;
    }
    if (a.tickPerMidiclock != b.tickPerMidiclock) {
        return a.tickPerMidiclock < b.tickPerMidiclock;
    }
    return a.num32ndNotePer24Midiclock < b.num32ndNotePer24Midiclock;
}
bool operator>(const TimeSignature& a, const TimeSignature& b) {
    return b < a;
}
bool operator<=(const TimeSignature& a, const TimeSignature& b) {
    return !(a > b);
}
bool operator>=(const TimeSignature& a, const TimeSignature& b) {
    return !(a < b);
}

using TsList = MidiEventList<TimeSignature>;

using TsMap = MidiEventMap<TimeSignature>;


}  // namespace GoldType::MidiParse
#endif
