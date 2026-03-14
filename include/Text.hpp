/********************************************************************************************************
 * File Name    : Text.hpp
 * Author       : Csrua / Gold_RsC
 * github       : Gold-RsC(https://github.com/Gold-RsC)
 * bilibili     : Csrua(https://space.bilibili.com/361846321)
 * Email        : 310106329@qq.com
 * Update Date  : 2026/03/06
 * Details      : README.md
 ********************************************************************************************************/
#ifndef TEXT_HPP
#define TEXT_HPP

#include "BasicMidiEvent.hpp"
#include <string>
namespace GoldType::MidiParse {

class Text : public BasicMidiEvent_Meta {
public:
    MidiMetaType type;
    std::string text;

public:
    static std::string get_typeName(MidiMetaType type) {
        std::string ret;
        switch (type) {
            case MidiMetaType::track_text:
                ret = "track_text";
                break;
            case MidiMetaType::song_copyright:
                ret = "song_copyright";
                break;
            case MidiMetaType::track_name:
                ret = "track_name";
                break;
            case MidiMetaType::instrument_name:
                ret = "instrument_name";
                break;
            case MidiMetaType::lyric:
                ret = "lyric";
                break;
            case MidiMetaType::marker:
                ret = "marker";
                break;
            case MidiMetaType::start_point:
                ret = "start_point";
                break;
            case MidiMetaType::program_name:
                ret = "program_name";
                break;
            case MidiMetaType::device_name:
                ret = "device_name";
                break;
            default:
                ret = "unknown";
                break;
        }
        return ret;
    }

public:
    Text(MidiTime _time = 0, MidiTimeMode _timeMode = MidiTimeMode::tick, MidiTrackNum _track = 0,
         MidiMetaType _type = MidiMetaType::track_text, const std::string& _text = "")
        : BasicMidiEvent_Meta(_time, _timeMode, _track),
          type(_type),
          text(_text) {
    }
    Text(const Text&) = default;
    ~Text(void) = default;

public:
    MidiErrorCode get_errorCode(void) const noexcept override {
        if (track & 0xF0) {
            return MidiErrorCode::event_track;
        }
        if ((uint8_t)type < 0x10 && (uint8_t)type > 0x00) {
            return MidiErrorCode::meta_type;
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
bool operator==(const Text& a, const Text& b) {
    return a.time == b.time && a.timeMode == b.timeMode && a.track == b.track && (uint8_t)a.type == (uint8_t)b.type &&
           a.text == b.text;
}
bool operator!=(const Text& a, const Text& b) {
    return !(a == b);
}
bool operator<(const Text& a, const Text& b) {
    if (a.timeMode != b.timeMode) {
        return a.timeMode < b.timeMode;
    }
    if (a.time != b.time) {
        return a.time < b.time;
    }
    if (a.track != b.track) {
        return a.track < b.track;
    }
    if (a.type != b.type) {
        return (uint8_t)a.type < (uint8_t)b.type;
    }
    return a.text < b.text;
}
bool operator>(const Text& a, const Text& b) {
    return b < a;
}
bool operator<=(const Text& a, const Text& b) {
    return !(a > b);
}
bool operator>=(const Text& a, const Text& b) {
    return !(a < b);
}

using TextList = MidiEventList<Text>;

using TextMap = MidiEventMap<Text>;

}  // namespace GoldType::MidiParse
#endif
