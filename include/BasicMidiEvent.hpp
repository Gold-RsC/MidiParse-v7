/********************************************************************************************************
 * File Name    : BasicMidiEvent.hpp
 * Author       : Csrua / Gold_RsC
 * github       : Gold-RsC(https://github.com/Gold-RsC)
 * bilibili     : Csrua(https://space.bilibili.com/361846321)
 * Email        : 310106329@qq.com
 * Update Date  : 2026/03/06
 * Details      : README.md
 ********************************************************************************************************/
#ifndef BASICMIDIEVENT_HPP
#define BASICMIDIEVENT_HPP
#include "MidiError.hpp"
#include <algorithm>
#include <vector>

namespace GoldType::MidiParse {
enum MidiTimeMode : bool {
    tick = 0,
    microsecond = 1
};

using MidiTime = uint64_t;

using MidiTrackNum = uint8_t;
using MidiChannelNum = uint8_t;

class BasicMidiEvent : public MidiObject {
public:
    static MidiTime __time_error_v;
    MidiTime time;
    MidiTimeMode timeMode : 1;
    MidiTrackNum track : 7;

public:
    BasicMidiEvent(MidiTime _time = 0, MidiTimeMode _timeMode = MidiTimeMode::tick, MidiTrackNum _track = 0)
        : time(_time),
          timeMode(_timeMode),
          track(_track) {
    }
    BasicMidiEvent(const BasicMidiEvent&) = default;
    virtual ~BasicMidiEvent(void) = default;

public:
    virtual MidiTime& tick(void) {
        if (timeMode == MidiTimeMode::tick) {
            return time;
        }
        return __time_error_v;
    }
    virtual const MidiTime& tick(void) const {
        if (timeMode == MidiTimeMode::tick) {
            return time;
        }
        return __time_error_v;
    }
    virtual MidiTime& microsecond(void) {
        if (timeMode == MidiTimeMode::microsecond) {
            return time;
        }
        return __time_error_v;
    }
    virtual const MidiTime& microsecond(void) const {
        if (timeMode == MidiTimeMode::microsecond) {
            return time;
        }
        return __time_error_v;
    }

    virtual MidiErrorCode get_errorCode(void) const noexcept = 0;
};
MidiTime BasicMidiEvent::__time_error_v = MidiTime(-1);
class BasicMidiEvent_Non : public BasicMidiEvent {
public:
    uint8_t channel : 4;

public:
    BasicMidiEvent_Non(MidiTime _time = 0, MidiTimeMode _timeMode = MidiTimeMode::tick, MidiTrackNum _track = 0,
                       MidiChannelNum _channel = 0)
        : BasicMidiEvent(_time, _timeMode, _track),
          channel(_channel) {
    }
    BasicMidiEvent_Non(const BasicMidiEvent_Non&) = default;
    virtual ~BasicMidiEvent_Non(void) = default;

public:
    virtual MidiErrorCode get_errorCode(void) const noexcept = 0;
};
class BasicMidiEvent_Meta : public BasicMidiEvent {
public:
    BasicMidiEvent_Meta(MidiTime _time = 0, MidiTimeMode _timeMode = MidiTimeMode::tick, MidiTrackNum _track = 0)
        : BasicMidiEvent(_time, _timeMode, _track) {
    }
    BasicMidiEvent_Meta(const BasicMidiEvent_Meta&) = default;
    virtual ~BasicMidiEvent_Meta(void) = default;

public:
    virtual MidiErrorCode get_errorCode(void) const noexcept = 0;
};

template <typename _MidiEvent>
class BasicMidiEventContainer : public MidiObject {
public:
    virtual MidiErrorCode get_errorCode(void) const noexcept = 0;
};
template <typename _MidiEvent>
class MidiEventList : public std::vector<_MidiEvent>, public BasicMidiEventContainer<_MidiEvent> {
public:
    using std::vector<_MidiEvent>::vector;

public:
    void to_abs(void) {
        uint64_t last_time = 0;
        for (size_t i = 0; i < this->size(); ++i) {
            last_time += this->operator[](i).time;
            this->operator[](i).time = last_time;
        }
    }
    void to_delta(void) {
        uint64_t last_time = 0;
        for (size_t i = 0; i < this->size(); ++i) {
            this->operator[](i).time -= last_time;
            last_time += this->operator[](i).time;
        }
    }
    MidiTimeMode get_timeMode(void) const {
        if (this->size()) {
            MidiTimeMode ret = this->front().timeMode;
            for (size_t i = 1; i < this->size(); ++i) {
                if (ret != this->operator[](i).timeMode) {
                    return MidiTimeMode::tick;
                }
            }
            return ret;
        }
        return MidiTimeMode::tick;
    }
    void sort(void) {
        std::sort(this->begin(), this->end());
    }

    MidiErrorCode get_errorCode(void) const noexcept final {
        MidiErrorCode ret = MidiErrorCode::no_error;
        for (size_t i = 0; i < this->size(); ++i) {
            ret = this->operator[](i).get_errorCode();
            if (ret != MidiErrorCode::no_error) {
                return ret;
            }
        }
        return _midiError(ret);
    }
};
template <typename _MidiEvent>
class MidiEventMap : public std::vector<MidiEventList<_MidiEvent>>, public BasicMidiEventContainer<_MidiEvent> {
public:
    using std::vector<MidiEventList<_MidiEvent>>::vector;

public:
    void to_abs(void) {
        for (size_t i = 0; i < this->size(); ++i) {
            this->operator[](i).to_abs();
        }
    }
    void to_delta(void) {
        for (size_t i = 0; i < this->size(); ++i) {
            this->operator[](i).to_delta();
        }
    }
    MidiTimeMode get_timeMode(void) const {
        if (this->size()) {
            MidiTimeMode ret = this->front().get_timeMode();
            for (size_t i = 1; i < this->size(); ++i) {
                if (ret != this->operator[](i).get_timeMode()) {
                    return MidiTimeMode::tick;
                }
            }
            return ret;
        }
        return MidiTimeMode::tick;
    }
    void sort(void) {
        for (size_t i = 0; i < this->size(); ++i) {
            this->operator[](i).sort();
        }
    }
    MidiErrorCode get_errorCode(void) const noexcept final {
        MidiErrorCode ret = MidiErrorCode::no_error;
        for (size_t i = 0; i < this->size(); ++i) {
            ret = this->operator[](i).get_errorCode();
            if (ret != MidiErrorCode::no_error) {
                return ret;
            }
        }
        return ret;
    }
};

template <typename _MidiEvent>
MidiEventList<_MidiEvent> merge_event(const MidiEventMap<_MidiEvent>& _map) {
    size_t _size = 0;
    for (size_t i = 0; i < _map.size(); ++i) {
        _size += _map[i].size();
    }
    MidiEventList<_MidiEvent> ret;
    ret.reserve(_size);
    for (uint8_t trackIdx = 0; trackIdx < _map.size(); ++trackIdx) {
        ret.insert(ret.end(), _map[trackIdx].begin(), _map[trackIdx].end());
    }
    return ret;
}
template <typename _MidiEvent>
MidiEventMap<_MidiEvent> split_event(const MidiEventList<_MidiEvent>& _list) {
    size_t _size[128] = {0};
    uint8_t _max_track = 0;
    for (size_t i = 0; i < _list.size(); ++i) {
        ++_size[_list[i].track];
        _max_track = std::max(_list[i].track, _max_track);
    }
    MidiEventMap<_MidiEvent> ret(_max_track + 1);
    for (size_t i = 0; i <= _max_track; ++i) {
        ret[i].reserve(_size[i]);
    }
    for (size_t i = 0; i < _list.size(); ++i) {
        ret[_list[i].track].emplace_back(_list[i]);
    }
    return ret;
}

template <typename _MidiEvent>
MidiTrackNum get_ntracks(const MidiEventList<_MidiEvent>& _list) {
    uint8_t _num = 0;
    for (typename MidiEventList<_MidiEvent>::iterator it = _list.begin(); it != _list.end(); ++it) {
        _num = std::max(_num, it->track);
    }
    return _num + 1;
}
template <typename _MidiEvent>
MidiTrackNum get_ntracks(const MidiEventMap<_MidiEvent>& _map) {
    return _map.size();
}

template <typename _MidiEvent, typename _Fun, typename... _Args>
void for_event(MidiEventList<_MidiEvent>& _list, _Fun&& _fun, _Args&&... _args) {
    for (size_t eventIdx = 0; eventIdx < _list.size(); ++eventIdx) {
        _fun(_list[eventIdx], std::forward(_args)...);
    }
}
template <typename _MidiEvent, typename _Fun, typename... _Args>
void for_event(const MidiEventList<_MidiEvent>& _list, _Fun&& _fun, _Args&&... _args) {
    for (size_t eventIdx = 0; eventIdx < _list.size(); ++eventIdx) {
        _fun(_list[eventIdx], std::forward(_args)...);
    }
}
template <typename _MidiEvent, typename _Fun, typename... _Args>
void for_event(MidiEventMap<_MidiEvent>& _map, _Fun&& _fun, _Args&&... _args) {
    for (uint8_t trackIdx = 0; trackIdx < _map.size(); ++trackIdx) {
        for (size_t eventIdx = 0; eventIdx < _map[trackIdx].size(); ++eventIdx) {
            _fun(_map[trackIdx][eventIdx], std::forward(_args)...);
        }
    }
}
template <typename _MidiEvent, typename _Fun, typename... _Args>
void for_event(const MidiEventMap<_MidiEvent>& _map, _Fun&& _fun, _Args&&... _args) {
    for (uint8_t trackIdx = 0; trackIdx < _map.size(); ++trackIdx) {
        for (size_t eventIdx = 0; eventIdx < _map[trackIdx].size(); ++eventIdx) {
            _fun(_map[trackIdx][eventIdx], std::forward(_args)...);
        }
    }
}
template <typename _MidiEvent, typename _Fun, typename... _Args>
MidiEventList<_MidiEvent> filter_event(const MidiEventList<_MidiEvent>& _list, _Fun&& _fun, _Args&&... _args) {
    MidiEventList<_MidiEvent> ret;
    for (size_t i = 0; i < _list.size(); ++i) {
        if (_fun(_list[i], std::forward(_args)...)) {
            ret.emplace_back(_list[i]);
        }
    }
    return ret;
}
template <typename _MidiEvent, typename _Fun, typename... _Args>
MidiEventMap<_MidiEvent> filter_event(const MidiEventMap<_MidiEvent>& _map, _Fun&& _fun, _Args&&... _args) {
    MidiEventMap<_MidiEvent> ret(_map.size());
    for (uint8_t trackIdx = 0; trackIdx < _map.size(); ++trackIdx) {
        ret[trackIdx] = filter_event(_map[trackIdx], std::forward(_fun), std::forward(_args)...);
    }
    return ret;
}
template <typename _MidiEvent, typename _Fun, typename... _Args>
size_t count_event(const MidiEventList<_MidiEvent>& _list, _Fun&& _fun, _Args&&... _args) {
    size_t ret = 0;
    for (size_t i = 0; i < _list.size(); ++i) {
        if (_fun(_list[i], std::forward(_args)...)) {
            ++ret;
        }
    }
    return ret;
}
template <typename _MidiEvent, typename _Fun, typename... _Args>
size_t count_event(const MidiEventMap<_MidiEvent>& _map, _Fun&& _fun, _Args&&... _args) {
    size_t ret = 0;
    for (uint8_t trackIdx = 0; trackIdx < _map.size(); ++trackIdx) {
        ret += count_event(_map[trackIdx], std::forward(_fun), std::forward(_args)...);
    }
    return ret;
}
}  // namespace GoldType::MidiParse
#endif
