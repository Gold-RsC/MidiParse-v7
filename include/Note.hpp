/********************************************************************************************************
 * File Name    : Note.hpp
 * Author       : Csrua / Gold_RsC
 * github       : Gold-RsC(https://github.com/Gold-RsC)
 * bilibili     : Csrua(https://space.bilibili.com/361846321)
 * Email        : 310106329@qq.com
 * Update Date  : 2026/03/06
 * Details      : README.md
 ********************************************************************************************************/
#ifndef NOTE_HPP
#define NOTE_HPP
#include "BasicMidiEvent.hpp"

namespace GoldType::MidiParse {

class Note : public BasicMidiEvent_Non {
public:
    uint8_t pitch;
    uint8_t velocity;
    uint8_t instrument;
    double bar;
    double beat;

public:
    Note(MidiTime _time = 0, MidiTimeMode _timeMode = MidiTimeMode::tick, MidiTrackNum _track = 0,
         MidiChannelNum _channel = 0, uint8_t _pitch = 0, uint8_t _velocity = 0, uint8_t _instrument = 0,
         double _bar = 0, double _beat = 0)
            : BasicMidiEvent_Non(_time, _timeMode, _track, _channel),
              pitch(_pitch),
              velocity(_velocity),
              instrument(_instrument),
              bar(_bar),
              beat(_beat) {
    }
    Note(const Note&) = default;
    ~Note(void) = default;

    bool is_empty(void) const {
        return !(time) && !(channel) && !(track) && !(pitch) && !(velocity) && (!instrument);
    }
    MidiErrorType get_error(MidiError& _midiError) const override {
        return MidiErrorType::no_error;
    }
};
bool operator==(const Note& a, const Note& b) {
    return a.time == b.time && a.timeMode == b.timeMode && a.channel == b.channel && a.track == b.track &&
           a.pitch == b.pitch && a.velocity == b.velocity && a.instrument == b.instrument;
}
bool operator!=(const Note& a, const Note& b) {
    return !(a == b);
}
bool operator<(const Note& a, const Note& b) {
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
    if (a.velocity != b.velocity) {
        return a.velocity < b.velocity;
    }
    if (a.pitch != b.pitch) {
        return a.pitch < b.pitch;
    }
    return a.instrument < b.instrument;
}
bool operator>(const Note& a, const Note& b) {
    return b < a;
}
bool operator<=(const Note& a, const Note& b) {
    return !(a > b);
}
bool operator>=(const Note& a, const Note& b) {
    return !(a < b);
}

using NoteList = MidiEventList<Note>;

using NoteMap = MidiEventMap<Note>;

}  // namespace GoldType::MidiParse
#endif
