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
#include <unordered_map>
namespace GoldType::MidiParse {
namespace Pitch {
const double A4_freq = 440.0;

double to_freq(uint8_t pitch) {
    return A4_freq * std::pow(2.0, (pitch - 69) / 12.0);
}
uint8_t from_freq(double freq) {
    return 69 + std::round(12 * std::log2(freq / A4_freq));
}

// 基础音名定义
const std::vector<std::string> sharpNames = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
const std::vector<std::string> flatNames = {"C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B"};
// 调号定义
enum class KeySignature {
    C_MAJOR = 0,        // C大调/A小调
    G_MAJOR = 1,        // G大调/E小调 (1个升号)
    D_MAJOR = 2,        // D大调/B小调 (2个升号)
    A_MAJOR = 3,        // A大调/F#小调 (3个升号)
    E_MAJOR = 4,        // E大调/C#小调 (4个升号)
    B_MAJOR = 5,        // B大调/G#小调 (5个升号)
    F_SHARP_MAJOR = 6,  // F#大调/D#小调 (6个升号)
    C_SHARP_MAJOR = 7,  // C#大调/A#小调 (7个升号)
    F_MAJOR = -1,       // F大调/D小调 (1个降号)
    B_FLAT_MAJOR = -2,  // Bb大调/G小调 (2个降号)
    E_FLAT_MAJOR = -3,  // Eb大调/C小调 (3个降号)
    A_FLAT_MAJOR = -4,  // Ab大调/F小调 (4个降号)
    D_FLAT_MAJOR = -5,  // Db大调/Bb小调 (5个降号)
    G_FLAT_MAJOR = -6,  // Gb大调/Eb小调 (6个降号)
    C_FLAT_MAJOR = -7   // Cb大调/Ab小调 (7个降号)
};
// 调号对应的升降音列表
const std::unordered_map<KeySignature, std::vector<uint8_t>> keyAccidentals = {
    {KeySignature::C_MAJOR, {}},
    {KeySignature::G_MAJOR, {6}},                           // F#
    {KeySignature::D_MAJOR, {6, 1}},                        // F#, C#
    {KeySignature::A_MAJOR, {6, 1, 8}},                     // F#, C#, G#
    {KeySignature::E_MAJOR, {6, 1, 8, 3}},                  // F#, C#, G#, D#
    {KeySignature::B_MAJOR, {6, 1, 8, 3, 10}},              // F#, C#, G#, D#, A#
    {KeySignature::F_SHARP_MAJOR, {6, 1, 8, 3, 10, 5}},     // F#, C#, G#, D#, A#, E#
    {KeySignature::C_SHARP_MAJOR, {6, 1, 8, 3, 10, 5, 0}},  // F#, C#, G#, D#, A#, E#, B#
    {KeySignature::F_MAJOR, {5}},                           // Bb
    {KeySignature::B_FLAT_MAJOR, {5, 10}},                  // Bb, Eb
    {KeySignature::E_FLAT_MAJOR, {5, 10, 3}},               // Bb, Eb, Ab
    {KeySignature::A_FLAT_MAJOR, {5, 10, 3, 8}},            // Bb, Eb, Ab, Db
    {KeySignature::D_FLAT_MAJOR, {5, 10, 3, 8, 1}},         // Bb, Eb, Ab, Db, Gb
    {KeySignature::G_FLAT_MAJOR, {5, 10, 3, 8, 1, 6}},      // Bb, Eb, Ab, Db, Gb, Cb
    {KeySignature::C_FLAT_MAJOR, {5, 10, 3, 8, 1, 6, 11}}   // Bb, Eb, Ab, Db, Gb, Cb, Fb
};
// 基础音高转换函数
std::string to_name(uint8_t pitch, bool useFlats = false) {
    int octave = pitch / 12 - 1;  // MIDI音高60(C4)对应八度4
    int noteIndex = pitch % 12;
    const auto& names = useFlats ? flatNames : sharpNames;
    return names[noteIndex] + std::to_string(octave);
}
// 根据调号转换音高到唱名
std::string to_name_with_key(uint8_t pitch, KeySignature key = KeySignature::C_MAJOR) {
    int octave = pitch / 12 - 1;
    int noteIndex = pitch % 12;

    // 检查该音是否在调号的升降音列表中
    auto it = keyAccidentals.find(key);
    if (it != keyAccidentals.end()) {
        const auto& accidentals = it->second;
        if (std::find(accidentals.begin(), accidentals.end(), noteIndex) != accidentals.end()) {
            // 使用升号或降号取决于调号类型
            if (static_cast<int>(key) >= 0) {
                return sharpNames[noteIndex] + std::to_string(octave);
            }
            else {
                return flatNames[noteIndex] + std::to_string(octave);
            }
        }
    }

    // 默认情况下，对于升号调使用升号，降号调使用降号
    if (static_cast<int>(key) >= 0) {
        return sharpNames[noteIndex] + std::to_string(octave);
    }
    else {
        return flatNames[noteIndex] + std::to_string(octave);
    }
}
// 唱名转音高函数
uint8_t from_name(const std::string& name) {
    static const std::unordered_map<std::string, int> noteMap = {
        {"C", 0},   {"C#", 1},  {"Db", 1}, {"D", 2},   {"D#", 3}, {"Eb", 3}, {"E", 4},
        {"F", 5},   {"F#", 6},  {"Gb", 6}, {"G", 7},   {"G#", 8}, {"Ab", 8}, {"A", 9},
        {"A#", 10}, {"Bb", 10}, {"B", 11}, {"Cb", 11}, {"B#", 0}, {"E#", 5}, {"Fb", 4}};

    // 解析音名和八度
    std::string notePart;
    int octave = 4;  // 默认八度

    // 查找数字部分
    size_t digitPos = name.find_first_of("0123456789");
    if (digitPos != std::string::npos) {
        notePart = name.substr(0, digitPos);
        try {
            octave = std::stoi(name.substr(digitPos));
        }
        catch (...) {
            octave = 4;
        }
    }
    else {
        notePart = name;
    }

    // 查找音名
    auto it = noteMap.find(notePart);
    if (it != noteMap.end()) {
        int basePitch = it->second;
        return static_cast<uint8_t>((octave + 1) * 12 + basePitch);
    }

    return 0xFF;
}
}  // namespace Pitch
namespace Velocity {
enum class VelocityMark : uint8_t {
    off = 0x00,
    ppp = 0x0F,
    pp = 0x1F,
    p = 0x2F,
    mp = 0x3F,
    mf = 0x4F,
    f = 0x5F,
    ff = 0x6F,
    fff = 0x7F,
    unknown = 0xFF,
};
std::string to_mark_str(uint8_t velocity) {
    if (velocity == (uint8_t)VelocityMark::off) {
        return "off";
    }
    if (velocity <= (uint8_t)VelocityMark::ppp) {
        return "ppp";
    }
    if (velocity <= (uint8_t)VelocityMark::pp) {
        return "pp";
    }
    if (velocity <= (uint8_t)VelocityMark::p) {
        return "p";
    }
    if (velocity <= (uint8_t)VelocityMark::mp) {
        return "mp";
    }
    if (velocity <= (uint8_t)VelocityMark::mf) {
        return "mf";
    }
    if (velocity <= (uint8_t)VelocityMark::f) {
        return "f";
    }
    if (velocity <= (uint8_t)VelocityMark::ff) {
        return "ff";
    }
    if (velocity <= (uint8_t)VelocityMark::fff) {
        return "fff";
    }
    return "unknown";
}
uint8_t from_mark_str(const std::string& str) {
    if (str == "off") {
        return (uint8_t)VelocityMark::off;
    }
    if (str == "ppp") {
        return (uint8_t)VelocityMark::ppp;
    }
    if (str == "pp") {
        return (uint8_t)VelocityMark::pp;
    }
    if (str == "p") {
        return (uint8_t)VelocityMark::p;
    }
    if (str == "mp") {
        return (uint8_t)VelocityMark::mp;
    }
    if (str == "mf") {
        return (uint8_t)VelocityMark::mf;
    }
    if (str == "f") {
        return (uint8_t)VelocityMark::f;
    }
    if (str == "ff") {
        return (uint8_t)VelocityMark::ff;
    }
    if (str == "fff") {
        return (uint8_t)VelocityMark::fff;
    }
    return (uint8_t)VelocityMark::unknown;
}
VelocityMark to_mark(uint8_t velocity) {
    if (velocity == (uint8_t)VelocityMark::off) {
        return VelocityMark::off;
    }
    if (velocity <= (uint8_t)VelocityMark::ppp) {
        return VelocityMark::ppp;
    }
    if (velocity <= (uint8_t)VelocityMark::pp) {
        return VelocityMark::pp;
    }
    if (velocity <= (uint8_t)VelocityMark::p) {
        return VelocityMark::p;
    }
    if (velocity <= (uint8_t)VelocityMark::mp) {
        return VelocityMark::mp;
    }
    if (velocity <= (uint8_t)VelocityMark::mf) {
        return VelocityMark::mf;
    }
    if (velocity <= (uint8_t)VelocityMark::f) {
        return VelocityMark::f;
    }
    if (velocity <= (uint8_t)VelocityMark::ff) {
        return VelocityMark::ff;
    }
    if (velocity <= (uint8_t)VelocityMark::fff) {
        return VelocityMark::fff;
    }
    return VelocityMark::unknown;
}
uint8_t from_mark(VelocityMark mark) {
    return static_cast<uint8_t>(mark);
}
double nomalize(uint8_t velocity) {
    return static_cast<double>(velocity) / 127.0;
}
uint8_t denormalize(double velocity) {
    return static_cast<uint8_t>(velocity * 127.0);
}
}  // namespace Velocity


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
