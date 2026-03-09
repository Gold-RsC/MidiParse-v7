/********************************************************************************************************
 * File Name    : BarBeat.hpp
 * Author       : Csrua / Gold_RsC
 * github       : Gold-RsC(https://github.com/Gold-RsC)
 * bilibili     : Csrua(https://space.bilibili.com/361846321)
 * Email        : 310106329@qq.com
 * Update Date  : 2026/03/06
 * Details      : README.md
 ********************************************************************************************************/
#ifndef BARBEAT_HPP
#define BARBEAT_HPP
#include "TimeSignature.hpp"

namespace GoldType::MidiParse {
class BarBeat : public TimeSignature {
public:
    double barNode;
    double beatNode;

public:
    BarBeat(uint64_t _time = 0, MidiTimeMode _timeMode = MidiTimeMode::tick, uint8_t _track = 0, double _barNode = 0,
            double _beatNode = 0, uint8_t _numerator = 4, uint8_t _denominator = 4, uint8_t _tickPerMidiclock = 24,
            uint8_t _num32ndNotePer24Midiclock = 8)
        : TimeSignature(_time, _timeMode, _track, _numerator, _denominator, _tickPerMidiclock,
                        _num32ndNotePer24Midiclock),
          barNode(_barNode),
          beatNode(_beatNode) {
    }
    BarBeat(const BarBeat&) = default;
    ~BarBeat(void) = default;

public:
    MidiErrorCode get_errorCode(void) const noexcept final {
        return TimeSignature::get_errorCode();
    }
};
bool operator==(const BarBeat& a, const BarBeat& b) {
    return a.time == b.time && a.timeMode == b.timeMode && a.track == b.track && a.barNode == b.barNode &&
           a.beatNode == b.beatNode;
}
bool operator!=(const BarBeat& a, const BarBeat& b) {
    return !(a == b);
}
bool operator<(const BarBeat& a, const BarBeat& b) {
    if (a.timeMode != b.timeMode) {
        return a.timeMode < b.timeMode;
    }
    if (a.time != b.time) {
        return a.time < b.time;
    }
    if (a.track != b.track) {
        return a.track < b.track;
    }
    if (a.barNode != b.barNode) {
        return a.barNode < b.barNode;
    }
    if (a.barNode != b.barNode) {
        return a.barNode < b.barNode;
    }
    if (a.numerator != b.numerator) {
        return a.numerator < b.numerator;
    }
    return a.denominator < b.denominator;
}
bool operator>(const BarBeat& a, const BarBeat& b) {
    return b < a;
}
bool operator<=(const BarBeat& a, const BarBeat& b) {
    return !(a > b);
}
bool operator>=(const BarBeat& a, const BarBeat& b) {
    return !(a < b);
}

using BarBeatList = MidiEventList<BarBeat>;

using BarBeatMap = MidiEventMap<BarBeat>;

TsList bbList_to_tsList(const BarBeatList& bbList) {
    TsList tsList;
    for (const auto& barBeat : bbList) {
        tsList.emplace_back((TimeSignature)barBeat);
    }
    return tsList;
}
TsMap bbMap_to_tsMap(const BarBeatMap& bbMap) {
    TsMap tsMap;
    for (const auto& bbList : bbMap) {
        tsMap.emplace_back(bbList_to_tsList(bbList));
    }
    return tsMap;
}

}  // namespace GoldType::MidiParse
#endif
