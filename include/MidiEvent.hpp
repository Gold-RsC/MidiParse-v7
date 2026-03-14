/********************************************************************************************************
 * File Name    : MidiEvent.hpp
 * Author       : Csrua / Gold_RsC
 * github       : Gold-RsC(https://github.com/Gold-RsC)
 * bilibili     : Csrua(https://space.bilibili.com/361846321)
 * Email        : 310106329@qq.com
 * Update Date  : 2026/03/06
 * Details      : README.md
 ********************************************************************************************************/
#ifndef MIDIEVENT_HPP
#define MIDIEVENT_HPP
#include "BasicMidiEvent.hpp"
#include <algorithm>
namespace GoldType::MidiParse {
enum class MidiEventType : uint8_t {
    null = 0x00,
    unknown = 0x7F,
    /******************
    Part1:type = 0xNx (N = 8~E ,x = 0~F)
    type & 0xF0 = 0xN0
    ******************/
    note_off = 0x80,            // 2
    note_on = 0x90,             // 2
    key_afterTouch = 0xA0,      // 2
    controller = 0xB0,          // 2
    program = 0xC0,             // 1
    channel_afterTouch = 0xD0,  // 1
    pitchWheel = 0xE0,          // 2

    /*******************
    Part2:type = 0xF0 or 0xF7
    *******************/
    sysex_begin = 0xF0,
    sysex_end = 0xF7,

    /*******************
    Part3:type = 0xFF
    *******************/
    meta = 0xFF
};
enum class MidiMetaType : uint8_t {
    // 00
    track_order = 0x00,
    // 0x:Text
    track_text = 0x01,
    song_copyright = 0x02,
    track_name = 0x03,
    instrument_name = 0x04,
    lyric = 0x05,
    marker = 0x06,
    start_point = 0x07,
    program_name = 0x08,
    device_name = 0x09,
    // 2x
    midi_channel = 0x20,
    midi_port = 0x21,
    end_of_track = 0x2F,
    // 5x
    tempo = 0x51,
    time_signature = 0x58,
    key_signature = 0x59,
    // 7F
    specific_info = 0x7F,
    // FF
    unknown = 0xFF,
};
using MidiByte = uint8_t;
class MidiMessage : public std::vector<MidiByte>, public MidiObject {
public:
    using std::vector<MidiByte>::vector;
    MidiEventType type(void) const {
        if (operator[](0) == 0xFF || operator[](0) == 0xF0 || operator[](0) == 0xF7) {
            return MidiEventType(operator[](0));
        }
        else if (operator[](0) >= 0x80 && operator[](0) < 0xF0) {
            return MidiEventType(operator[](0) & 0xF0);
        }
        return_or_throw(MidiEventType::unknown, MidiError(MidiErrorCode::event_unknown_type, 0xFF, 0xFFFFFFFF));
    }
    MidiMetaType meta_type(void) const {
        return_or_throw_ignorably_if(!is_meta(), MidiMetaType::unknown,
                                     MidiError(MidiErrorCode::meta_type, 0xFF, 0xFFFFFFFF));
        return MidiMetaType(operator[](1));
    }
    MidiChannelNum channel(void) const {
        return_or_throw_ignorably_if(operator[](0) >= 0xF0 || operator[](0) < 0x80, 0xFF,
                                     MidiError(MidiErrorCode::event_channel, 0xFF, 0xFFFFFFFF));
        return operator[](0) & 0x0F;
    }

public:
    bool is_normal(void) const {
        return !(is_meta() || is_sysex());
    }
    bool is_meta(void) const {
        return type() == MidiEventType::meta;
    }
    bool is_sysex(void) const {
        return type() == MidiEventType::sysex_begin || type() == MidiEventType::sysex_end;
    }

public:
    MidiErrorCode get_errorCode(void) const noexcept final {
        MidiEventType _type = type();
        switch (_type) {
            case MidiEventType::note_off:
            case MidiEventType::note_on:
            case MidiEventType::key_afterTouch:
            case MidiEventType::controller:
            case MidiEventType::pitchWheel: {
                if (operator[](1) & 0x80) {
                    return gennerate_errorCode_from_eventInfo((uint8_t)_type, 0x01);
                }
                if (operator[](2) & 0x80) {
                    return gennerate_errorCode_from_eventInfo((uint8_t)_type, 0x02);
                }
                break;
            }
            case MidiEventType::program:
            case MidiEventType::channel_afterTouch: {
                if (operator[](1) & 0x80) {
                    return gennerate_errorCode_from_eventInfo((uint8_t)_type, 0x01);
                }
                break;
            }
            case MidiEventType::sysex_begin:
            case MidiEventType::sysex_end: {
                size_t i = 1;
                uint32_t length = 0;
                for (; i < 5; ++i) {
                    length = (length << 7) | (operator[](i) & 0x7F);
                    if (operator[](i) < 0x80) {
                        break;
                    }
                }
                if (i + length + 1 != size()) {
                    return MidiErrorCode::sysex_length;
                }
                break;
            }
            case MidiEventType::meta: {
                size_t i = 2;
                uint32_t length = 0;
                for (; i < 5; ++i) {
                    length = (length << 7) | (operator[](i) & 0x7F);
                    if (operator[](i) < 0x80) {
                        break;
                    }
                }
                if (i + length + 1 != size()) {
                    return MidiErrorCode::meta_length;
                }
                break;
            }
            default: {
                return MidiErrorCode::event_unknown_type;
            }
        }
        return MidiErrorCode::no_error;
    }
    MidiError get_error(void) const noexcept final {
        return MidiError(get_errorCode(), 0xFF, 0xFFFFFFFF);
    }
};

class MidiEvent : public BasicMidiEvent {
public:
    MidiMessage message;

public:
    MidiEvent(void)
        : message() {
    }
    MidiEvent(const MidiEvent& another) = default;
    MidiEvent(MidiTime _time, MidiTimeMode _mode, MidiTrackNum _track, const MidiMessage& _message)
        : BasicMidiEvent(_time, _mode, _track),
          message(_message) {
    }
    ~MidiEvent(void) = default;

public:
    MidiEventType type(void) const {
        return message.type();
    }
    MidiMetaType meta_type(void) const {
        return message.meta_type();
    }
    MidiChannelNum channel(void) const {
        return message.channel();
    }

public:
    bool is_normal(void) const {
        return !(is_meta() || is_sysex());
    }
    bool is_meta(void) const {
        return type() == MidiEventType::meta;
    }
    bool is_sysex(void) const {
        return type() == MidiEventType::sysex_begin || type() == MidiEventType::sysex_end;
    }
    MidiErrorCode get_errorCode(void) const noexcept final {
        return message.get_errorCode();
    }
    MidiError get_error(void) const noexcept final {
        return MidiError(message.get_errorCode(), track, 0xFFFFFFFF);
    }

public:
    MidiByte& at(size_t idx) {
        return message.at(idx);
    }
    const MidiByte& at(size_t idx) const {
        return message.at(idx);
    }

    MidiByte& operator[](size_t idx) noexcept {
        return message[idx];
    }
    const MidiByte& operator[](size_t idx) const noexcept {
        return message[idx];
    }
    MidiEvent& operator=(const MidiEvent& another) = default;
};
}  // namespace GoldType::MidiParse
#endif
