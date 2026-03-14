/********************************************************************************************************
 * File Name    : MidiError.hpp
 * Author       : Csrua / Gold_RsC
 * github       : Gold-RsC(https://github.com/Gold-RsC)
 * bilibili     : Csrua(https://space.bilibili.com/361846321)
 * Email        : 310106329@qq.com
 * Update Date  : 2026/03/06
 * Details      : README.md
 ********************************************************************************************************/
#ifndef MIDIERROR_HPP
#define MIDIERROR_HPP
#include <exception>
#include <iomanip>
#include <sstream>
#include <stdint.h>
#include <stdio.h>
#include <string>

#ifdef MIDI_DEBUG
#define MIDI_CHECK_LEVEL 3
#endif
#ifndef MIDI_CHECK_LEVEL
#define MIDI_CHECK_LEVEL 1
#endif
// no checking: FAST! FAST! FAST!!!
#if MIDI_CHECK_LEVEL >= 0
#endif
// check but not throw exceptions: SAFE AND FAST!
#if MIDI_CHECK_LEVEL >= 1
#define MIDI_ERROR_LEVEL_1
#endif

// able to throw simple exceptions: SAFE AND CLEAR!
#if MIDI_CHECK_LEVEL >= 2
#define MIDI_ERROR_LEVEL_2
#endif

// able to throw all exceptions: SAFE BUT SLOW!
#if MIDI_CHECK_LEVEL >= 3
#define MIDI_ERROR_LEVEL_3
#endif


namespace GoldType::MidiParse {

enum class MidiErrorCode : uint32_t {
    no_error = 0x00000000,
    filename = 0x00000001,

    head_identification = 0x00000100,
    head_length = 0x00000101,
    head_format = 0x00000102,
    head_ntracks = 0x00000103,
    head_division = 0x00000104,

    track_identification = 0x00000200,
    track_length = 0x00000201,

    event_deltaTime = 0x00000300,
    event_channel = 0x00000301,
    event_unknown_type = 0x00000302,
    event_track = 0x00000303,

    noteOff_pitch = 0x00008001,
    noteOff_velocity = 0x00008002,
    noteOn_pitch = 0x00009001,
    noteOn_velocity = 0x00009002,
    keyAfterTouch_pitch = 0x0000A001,
    keyAfterTouch_velocity = 0x0000A002,
    controller_number = 0x0000B001,
    controller_value = 0x0000B002,
    program_number = 0x0000C001,
    channelAfterTouch_velocity = 0x0000D001,
    pitchWheel_mod = 0x0000E001,
    pitchWheel_div = 0x0000E002,
    sysex_length = 0x0000F001,
    sysex_data = 0x0000F002,
    meta_type = 0x0000FF81,
    meta_length = 0x0000FF82,
    meta_data = 0x0000FF83,

    parse_error = 0x00000010,
    change_timeMode = 0x00000011,

    unknown_error = 0xFFFFFFFF,
};
struct MidiError {
    MidiErrorCode code;
    uint8_t track_idx;
    uint64_t event_idx;
    MidiError(void)
        : code(MidiErrorCode::no_error),
          track_idx(0xFF),
          event_idx(0xFFFFFFFF) {
    }
    MidiError(MidiErrorCode _code, uint8_t _track_idx, uint64_t _event_idx)
        : code(_code),
          track_idx(_track_idx),
          event_idx(_event_idx) {
    }
};
MidiErrorCode gennerate_errorCode_from_eventInfo(uint8_t type, uint8_t idx) {
    return (MidiErrorCode)(uint32_t(type) << 8 | idx);
}

#ifdef MIDI_DEBUG
std::string parse_error(MidiError error) {
    auto errorSentence = [](MidiErrorCode code) -> std::string {
        code = (MidiErrorCode)(uint32_t(code) & 0x0000FFFF);
        switch (code) {
            case MidiErrorCode::no_error: {
                return "There is no error.";
            }
            case MidiErrorCode::filename: {
                return "Please make sure the file name is correct.";
            }
            case MidiErrorCode::head_identification: {
                return "Head identification is incorrect.";
            }
            case MidiErrorCode::head_length: {
                return "Head length is not 6.";
            }
            case MidiErrorCode::head_format: {
                return "Head format is not 0, 1 or 2.";
            }
            case MidiErrorCode::head_ntracks: {
                return "Head ntracks is not 0.";
            }
            case MidiErrorCode::head_division: {
                return "Head division is not 0.";
            }
            case MidiErrorCode::track_identification: {
                return "Track identification is incorrect.";
            }
            case MidiErrorCode::track_length: {
                return "Track length is incorrect.";
            }
            case MidiErrorCode::event_deltaTime: {
                return "Event deltaTime is incorrect.";
            }
            case MidiErrorCode::event_channel: {
                return "Event channel is incorrect.";
            }
            case MidiErrorCode::event_track: {
                return "Event track is incorrect.";
            }
            case MidiErrorCode::event_unknown_type: {
                return "Event type is unknown.";
            }
            case MidiErrorCode::noteOff_pitch: {
                return "NoteOff pitch is incorrect.";
            }
            case MidiErrorCode::noteOff_velocity: {
                return "NoteOff velocity is incorrect.";
            }
            case MidiErrorCode::noteOn_pitch: {
                return "NoteOn pitch is incorrect.";
            }
            case MidiErrorCode::noteOn_velocity: {
                return "NoteOn velocity is incorrect.";
            }
            case MidiErrorCode::keyAfterTouch_pitch: {
                return "KeyAfterTouch pitch is incorrect.";
            }
            case MidiErrorCode::keyAfterTouch_velocity: {
                return "KeyAfterTouch velocity is incorrect.";
            }
            case MidiErrorCode::controller_number: {
                return "Controller number is incorrect.";
            }
            case MidiErrorCode::controller_value: {
                return "Controller value is incorrect.";
            }
            case MidiErrorCode::program_number: {
                return "Program number is incorrect.";
            }
            case MidiErrorCode::channelAfterTouch_velocity: {
                return "ChannelAfterTouch velocity is incorrect.";
            }
            case MidiErrorCode::pitchWheel_mod: {
                return "PitchWheel mod is incorrect.";
            }
            case MidiErrorCode::pitchWheel_div: {
                return "PitchWheel div is incorrect.";
            }
            case MidiErrorCode::sysex_length: {
                return "Sysex length is incorrect.";
            }
            case MidiErrorCode::sysex_data: {
                return "Sysex data is incorrect.";
            }
            case MidiErrorCode::meta_length: {
                return "Meta length is incorrect.";
            }
            case MidiErrorCode::meta_type: {
                return "Meta type is incorrect.";
            }
            case MidiErrorCode::meta_data: {
                return "Meta data is incorrect.";
            }
            case MidiErrorCode::parse_error: {
                return "Parse error.";
            }
            case MidiErrorCode::change_timeMode: {
                return "An error occurred when changing timeMode.";
            }
        }
        return "Unknown error.";
    };
    std::stringstream ss;
    ss << "Midi Error:\n\t" << errorSentence(error.code);
    if (error.track_idx != 0xFF) {
        ss << "\n\tTrack Idx: " << (uint32_t)error.track_idx;
    }
    if (error.event_idx != 0xFFFFFFFF) {
        ss << "\n\tEvent Idx: " << error.event_idx;
    }
    ss << "\n\tMidiErrorCode:" << std::setw(sizeof(MidiErrorCode) * 2 + 2) << std::setfill('0') << std::internal
       << std::showbase << std::hex << std::uppercase << (uint32_t)error.code;
    return ss.str();
}
#else
std::string parse_error(MidiError error) {
    std::stringstream ss;
    ss << "[" << (uint32_t)error.track_idx << "," << (uint32_t)error.event_idx << "]"
       << std::setw(sizeof(MidiErrorCode) * 2 + 2) << std::setfill('0') << std::internal << std::showbase << std::hex
       << std::uppercase << (uint32_t)error.code;
    return ss.str();
}
#endif

class MidiException : public std::exception {
private:
    std::string m_msg;

public:
    MidiError error;

public:
    MidiException(MidiError _error)
        : error(_error),
          m_msg(parse_error(_error)) {
    }
    const char* what() const noexcept override {
        return m_msg.c_str();
    }
};

// return + ignorably          -> level 0 or 1
// return or throw             -> level 1 or 2
// return or throw ignorably   -> level 0 or 1 or 2
// throw  + ignorably          -> level 2 or 3
#if MIDI_CHECK_LEVEL >= 3
#define select_based_on_level(_sts_0, _sts_1, _sts_2, _sts_3) _sts_3
#elif MIDI_CHECK_LEVEL >= 2
#define select_based_on_level(_sts_0, _sts_1, _sts_2, _sts_3) _sts_2
#elif MIDI_CHECK_LEVEL >= 1
#define select_based_on_level(_sts_0, _sts_1, _sts_2, _sts_3) _sts_1
#else
#define select_based_on_level(_sts_0, _sts_1, _sts_2, _sts_3) _sts_0
#endif
#if MIDI_CHECK_LEVEL >= 1
#define select_above_level_1(_sts_true, _sts_false) _sts_true
#define return_ignorably(_content) return _content
#define return_ignorably_if(condition, _content)                                                                       \
    if (condition)                                                                                                     \
    return _content
#define return_or_throw_ignorably_if(condition, _ret, _thr)                                                            \
    if (condition)                                                                                                     \
    return_or_throw(_ret, _thr)
#else
#define select_above_level_1(_sts_true, _sts_false) _sts_false
#define return_ignorably(_code) ((void)0)
#define return_ignorably_if(condition, _code) ((void)0)
#define return_or_throw_ignorably_if(condition, _ret, _thr) ((void)0)
#endif

#if MIDI_CHECK_LEVEL >= 2
#define select_above_level_2(_sts_true, _sts_false) _sts_true
#define return_or_throw(_ret, _thr) throw MidiException(_thr)
#else
#define select_above_level_2(_sts_true, _sts_false) _sts_false
#define return_or_throw(_ret, _thr) return _ret
#endif

#if MIDI_CHECK_LEVEL >= 3
#define select_above_level_3(_sts_true, _sts_false) _sts_true
#define throw_ignorably(_thr) throw MidiException(_thr)
#define throw_ignorably_if(condition, _thr)                                                                            \
    if (condition)                                                                                                     \
    throw_ignorably(_thr)
#else
#define select_above_level_3(_sts_true, _sts_false) _sts_false
#define throw_ignorably(_thr) ((void)0)
#define throw_ignorably_if(condition, _thr) ((void)0)
#endif


class MidiObject {
public:
    virtual MidiErrorCode get_errorCode(void) const noexcept = 0;
    virtual MidiError get_error(void) const noexcept = 0;
};

}  // namespace GoldType::MidiParse
#endif
