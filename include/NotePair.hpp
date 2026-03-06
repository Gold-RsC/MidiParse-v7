/********************************************************************************************************
 * File Name    : NotePair.hpp
 * Author       : Csrua / Gold_RsC
 * github       : Gold-RsC(https://github.com/Gold-RsC)
 * bilibili     : Csrua(https://space.bilibili.com/361846321)
 * Email        : 310106329@qq.com
 * Update Date  : 2026/03/06
 * Details      : README.md
 ********************************************************************************************************/
#ifndef NOTEPAIR_HPP
#define NOTEPAIR_HPP
#include "Note.hpp"
#include <map>
#include <queue>
namespace GoldType::MidiParse {

bool is_notePair(const Note& _begin, const Note& _end) {
    return _begin.time < _end.time && _begin.timeMode == _end.timeMode && _begin.channel == _end.channel &&
           _begin.track == _end.track && _begin.pitch == _end.pitch && _begin.instrument == _end.instrument &&
           _begin.velocity && !(_end.velocity);
}
class NotePair : public Note {
public:
    uint64_t duration;
    double bar_diff;
    double beat_diff;

public:
    NotePair(MidiTime _time = 0, MidiTime _duration = 0, MidiTimeMode _timeMode = MidiTimeMode::tick,
             MidiTrackNum _track = 0, MidiChannelNum _channel = 0, uint8_t _pitch = 0, uint8_t _velocity = 0,
             uint8_t _instrument = 0, double _bar = 0, double _bar_diff = 0, double _beat = 0, double _beat_diff = 0)
            : Note(_time, _timeMode, _track, _channel, _pitch, _velocity, _instrument, _bar, _beat),
              duration(_duration),
              bar_diff(_bar_diff),
              beat_diff(_beat_diff) {
    }
    NotePair(const Note& _begin, const Note& _end) {
        if (is_notePair(_begin, _end)) {
            *this = NotePair(_begin.time, _end.time - _begin.time, _begin.timeMode, _begin.track, _begin.channel,
                             _begin.pitch, _begin.velocity, _begin.instrument, _begin.bar, _end.bar - _begin.bar,
                             _begin.beat, _end.beat - _begin.beat);
        }
        else {
            *this = NotePair();
        }
    }
    NotePair(const NotePair&) = default;
    ~NotePair(void) = default;

public:
    std::pair<Note, Note> devide(void) const {
        return std::pair<Note, Note>(Note(time, timeMode, track, channel, pitch, velocity, instrument, bar, beat),
                                     Note(time + duration, timeMode, track, channel, pitch, 0, instrument,
                                          bar + bar_diff, beat + beat_diff));
    }

public:
    MidiErrorType get_error(MidiError& _midiError) const final {
        return MidiErrorType::no_error;
    }
};
bool operator==(const NotePair& a, const NotePair& b) {
    return a.time == b.time && a.duration == b.duration && a.timeMode == b.timeMode && a.channel == b.channel &&
           a.track == b.track && a.pitch == b.pitch && a.velocity == b.velocity && a.instrument == b.instrument;
}
bool operator!=(const NotePair& a, const NotePair& b) {
    return !(a == b);
}
bool operator<(const NotePair& a, const NotePair& b) {
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
    if (a.pitch != b.pitch) {
        return a.pitch < b.pitch;
    }
    if (a.velocity != b.velocity) {
        return a.velocity < b.velocity;
    }
    return a.instrument < b.instrument;
}
bool operator>(const NotePair& a, const NotePair& b) {
    return b < a;
}
bool operator<=(const NotePair& a, const NotePair& b) {
    return !(a > b);
}
bool operator>=(const NotePair& a, const NotePair& b) {
    return !(a < b);
}


using NotePairList = MidiEventList<NotePair>;
using NotePairMap = MidiEventMap<NotePair>;

NotePairList link_notePair(const NoteList& _noteList) {
    NotePairList ret;
    ret.reserve(_noteList.size() / 2 + 1);
    std::map<uint32_t, std::queue<Note>> _map;
    for (NoteList::const_iterator it = _noteList.cbegin(); it != _noteList.cend(); ++it) {
        uint32_t _key =
                bool(it->timeMode) << 25 | it->track << 18 | it->channel << 14 | it->pitch << 7 | it->instrument;
        auto _map_it = _map.find(_key);
        if (_map_it == _map.end()) {
            if (it->velocity) {
                _map.insert({_key, std::queue<Note>({*it})});
            }
            else {
                printf("???");
            }
        }
        else {
            auto& _queue = _map_it->second;
            if (it->velocity) {
                _queue.emplace(*it);
            }
            else {
                ret.emplace_back(_queue.front(), *it);
                _queue.pop();
                if (_queue.empty()) {
                    _map.erase(_map_it);
                }
            }
        }
    }
    if (!_map.empty()) {
        printf("!!!");
    }
    return ret;
}
NotePairMap link_notePair(const NoteMap& _noteMap) {
    NotePairMap ret(_noteMap.size());
    for (size_t i = 0; i < _noteMap.size(); ++i) {
        ret[i] = link_notePair(_noteMap[i]);
    }
    return ret;
}
NoteList devide_notePair(const NotePairList& _list) {
    NoteList _l;
    _l.reserve(2 * _list.size());
    for (NotePairList::const_iterator it = _list.cbegin(); it != _list.cend(); ++it) {
        std::pair<Note, Note> _n = it->devide();
        _l.emplace_back(_n.first);
        _l.emplace_back(_n.second);
    }
    return _l;
}
NoteMap devide_notePair(const NotePairMap& _map) {
    NoteMap ret(_map.size());
    for (size_t i = 0; i < _map.size(); ++i) {
        ret[i] = devide_notePair(_map[i]);
    }
    return ret;
}

}  // namespace GoldType::MidiParse
#endif
