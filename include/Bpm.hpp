/********************************************************************************************************
 * File Name    : BarBeat.hpp
 * Author       : Csrua / Gold_RsC
 * github       : Gold-RsC(https://github.com/Gold-RsC)
 * bilibili     : Csrua(https://space.bilibili.com/361846321)
 * Email        : 310106329@qq.com
 * Update Date  : 2026/03/09
 * Details      : README.md
 ********************************************************************************************************/
#ifndef BPM_HPP
#define BPM_HPP
#include "Tempo.hpp"
#include "TimeSignature.hpp"
#include <algorithm>

namespace GoldType::MidiParse {
class Bpm : public BasicMidiEvent_Meta {
public:
    double bpm;

public:
    Bpm(uint64_t _time = 0, MidiTimeMode _timeMode = MidiTimeMode::tick, uint8_t _track = 0, double _bpm = 0)
        : BasicMidiEvent_Meta(_time, _timeMode, _track),
          bpm(_bpm) {
    }
    Bpm(const Tempo& _tempo, const TimeSignature& _timeSignature) {
        if (_tempo.timeMode == _timeSignature.timeMode && _tempo.track == _timeSignature.track) {
            *this = Bpm(std::max<MidiTime>(_tempo.time, _timeSignature.time), _timeSignature.timeMode,
                        _timeSignature.track, 6e7 * _timeSignature.denominator / _tempo.mispqn);
        }
        *this = Bpm();
    }
    Bpm(const Bpm&) = default;
    ~Bpm(void) = default;

public:
    bool is_empty(void) const {
        return time == 0 && timeMode == MidiTimeMode::tick && track == 0 && bpm == 0.0;
    }

public:
    MidiErrorCode get_errorCode(void) const noexcept final {
        if (track & 0xF0) {
            return MidiErrorCode::event_track;
        }
        return MidiErrorCode::meta_data;
    }
};
bool operator==(const Bpm& a, const Bpm& b) {
    return a.time == b.time && a.timeMode == b.timeMode && a.track == b.track && a.bpm == b.bpm;
}
bool operator!=(const Bpm& a, const Bpm& b) {
    return !(a == b);
}
bool operator<(const Bpm& a, const Bpm& b) {
    if (a.timeMode != b.timeMode) {
        return a.timeMode < b.timeMode;
    }
    if (a.time != b.time) {
        return a.time < b.time;
    }
    if (a.track != b.track) {
        return a.track < b.track;
    }
    return a.bpm < b.bpm;
}
bool operator>(const Bpm& a, const Bpm& b) {
    return b < a;
}
bool operator<=(const Bpm& a, const Bpm& b) {
    return !(a > b);
}
bool operator>=(const Bpm& a, const Bpm& b) {
    return !(a < b);
}

using BpmList = MidiEventList<Bpm>;

BpmList generate_bpmList(const TempoList& _tempoList, const TsList& _timeSignatureList) {
    BpmList bpmList;
    if (_tempoList.empty() || _timeSignatureList.empty()) {
        bpmList = BpmList();
        return bpmList;
    }
    auto ts_it = _timeSignatureList.begin();
    auto tempo_it = _tempoList.begin();
    for (;;) {
        MidiTime _time = std::max<MidiTime>(ts_it->time, tempo_it->time);
        if (bpmList.empty() || bpmList.back().time < _time) {
            bpmList.emplace_back(Bpm(*tempo_it, *ts_it));
        }
        else {
            bpmList.back() = Bpm(*tempo_it, *ts_it);
        }
        if (ts_it + 1 == _timeSignatureList.end() && tempo_it + 1 == _tempoList.end()) {
            break;
        }
        else if (ts_it + 1 != _timeSignatureList.end() && tempo_it + 1 != _tempoList.end()) {
            if ((ts_it + 1)->time < (tempo_it + 1)->time) {
                ++ts_it;
            }
            else if ((ts_it + 1)->time < (tempo_it + 1)->time) {
                ++tempo_it;
            }
            else {
                ++ts_it;
                ++tempo_it;
            }
        }
        else if (ts_it + 1 == _timeSignatureList.end()) {
            ++tempo_it;
        }
        else {
            ++ts_it;
        }
    }
    return bpmList;
}
/************************
 * ts     ......^........^....................................^................^...........
 * tempo  ..........^.............^...........^...............^...........^................
 * bpm    ..........|....|........|...........|...............|...........|....|...........
 ***********************/

using BpmMap = MidiEventMap<Bpm>;
BpmMap generate_bpmMap(const TempoMap& _tempoMap, const TsMap& _timeSignatureMap) {
    BpmMap bpmMap;
    MidiTrackNum _track = std::min<size_t>(_tempoMap.size(), _timeSignatureMap.size());
    for (MidiTrackNum i = 0; i < _track; ++i) {
        bpmMap.emplace_back(generate_bpmList(_tempoMap[i], _timeSignatureMap[i]));
    }
    return bpmMap;
}

}  // namespace GoldType::MidiParse
#endif
