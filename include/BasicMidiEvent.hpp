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
#include <unordered_set>
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
    virtual MidiError get_error(void) const noexcept = 0;
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
    virtual MidiError get_error(void) const noexcept = 0;
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
    virtual MidiError get_error(void) const noexcept = 0;
};

template <typename _MidiEvent>
class BasicMidiEventContainer : public MidiObject {
public:
    virtual MidiErrorCode get_errorCode(void) const noexcept = 0;
    virtual MidiError get_error(void) const noexcept = 0;
};

template <typename _MidiEvent>
class MidiEventMap;
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
    template <typename _Cmp = std::less<_MidiEvent>>
    void sort(_Cmp _cmp = _Cmp()) {
        std::sort(this->begin(), this->end(), _cmp);
    }
    template <typename _Fun, typename... _Args>
    void for_event(_Fun&& _fun, _Args&&... _args) {
        for (typename std::vector<_MidiEvent>::iterator it = this->begin(); it != this->end(); ++it) {
            _fun(*it, std::forward(_args)...);
        }
    }
    template <typename _Fun, typename... _Args>
    void for_event(_Fun&& _fun, _Args&&... _args) const {
        for (typename std::vector<_MidiEvent>::const_iterator it = this->begin(); it != this->end(); ++it) {
            _fun(*it, std::forward(_args)...);
        }
    }
    template <typename _Fun, typename... _Args>
    MidiEventList<_MidiEvent> filter_event_if(_Fun&& _fun, _Args&&... _args) const {
        MidiEventList<_MidiEvent> ret;
        for (typename std::vector<_MidiEvent>::const_iterator it = this->begin(); it != this->end(); ++it) {
            if (_fun(*it, _args...)) {
                ret.emplace_back(*it);
            }
        }
        return ret;
    }
    template <typename _Fun, typename... _Args>
    size_t count_event_if(_Fun&& _fun, _Args&&... _args) const {
        size_t ret = 0;
        for (size_t i = 0; i < this->size(); ++i) {
            if (_fun(this->operator[](i), std::forward(_args)...)) {
                ++ret;
            }
        }
        return ret;
    }
    MidiEventMap<_MidiEvent> split_event(void) const;
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
    MidiError get_error(void) const noexcept {
        MidiError err;
        err.code = MidiErrorCode::no_error;
        for (size_t i = 0; i < this->size(); ++i) {
            err.code = this->operator[](i).get_errorCode();
            if (err.code != MidiErrorCode::no_error) {
                err.track_idx = (uint8_t)this->operator[](i).track;
                err.event_idx = i;
                return err;
            }
        }
        return err;
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
    template <typename _Cmp = std::less<_MidiEvent>>
    void sort(_Cmp _cmp = _Cmp()) {
        for (typename std::vector<MidiEventList<_MidiEvent>>::iterator it = this->begin(); it != this->end(); ++it) {
            it->template sort<_Cmp>(_cmp);
        }
    }
    template <typename _Fun, typename... _Args>
    void for_event(_Fun&& _fun, _Args&&... _args) {
        for (typename std::vector<MidiEventList<_MidiEvent>>::iterator it = this->begin(); it != this->end(); ++it) {
            it->for_event(_fun, std::forward(_args)...);
        }
    }
    template <typename _Fun, typename... _Args>
    void for_event(_Fun&& _fun, _Args&&... _args) const {
        for (typename std::vector<MidiEventList<_MidiEvent>>::const_iterator it = this->begin(); it != this->end();
             ++it) {
            it->for_event(_fun, std::forward(_args)...);
        }
    }
    template <typename _Fun, typename... _Args>
    void for_list(_Fun&& _fun, _Args&&... _args) {
        for (typename std::vector<MidiEventList<_MidiEvent>>::iterator it = this->begin(); it != this->end(); ++it) {
            _fun(*it, std::forward(_args)...);
        }
    }
    template <typename _Fun, typename... _Args>
    void for_list(_Fun&& _fun, _Args&&... _args) const {
        for (typename std::vector<MidiEventList<_MidiEvent>>::const_iterator it = this->begin(); it != this->end();
             ++it) {
            _fun(*it, std::forward(_args)...);
        }
    }
    template <typename _Fun, typename... _Args>
    MidiEventMap<_MidiEvent> filter_event_if(_Fun&& _fun, _Args&&... _args) const {
        MidiEventMap<_MidiEvent> ret;
        for (typename std::vector<MidiEventList<_MidiEvent>>::const_iterator it = this->begin(); it != this->end();
             ++it) {
            ret.emplace_back(it->filter_event_if(_fun, std::forward(_args)...));
        }
        return ret;
    }
    template <typename _Fun, typename... _Args>
    size_t count_event_if(_Fun&& _fun, _Args&&... _args) const {
        size_t ret = 0;
        for (typename std::vector<MidiEventList<_MidiEvent>>::const_iterator it = this->begin(); it != this->end();
             ++it) {
            ret += it->count_event_if(_fun, std::forward(_args)...);
        }
        return ret;
    }
    MidiEventList<_MidiEvent> merge_event(void) const {
        size_t _size = 0;
        for (typename std::vector<MidiEventList<_MidiEvent>>::const_iterator it = this->begin(); it != this->end();
             ++it) {
            _size += it->size();
        }
        MidiEventList<_MidiEvent> ret;
        ret.reserve(_size);
        for (typename std::vector<MidiEventList<_MidiEvent>>::const_iterator it = this->begin(); it != this->end();
             ++it) {
            ret.insert(ret.end(), it->begin(), it->end());
        }
        return ret;
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
    MidiError get_error(void) const noexcept final {
        MidiError err;
        err.code = MidiErrorCode::no_error;
        for (size_t i = 0; i < this->size(); ++i) {
            err = this->operator[](i).get_error();
            if (err.code != MidiErrorCode::no_error) {
                return err;
            }
        }
        return err;
    }
};

template <typename _MidiEvent>
MidiEventMap<_MidiEvent> MidiEventList<_MidiEvent>::split_event(void) const {
    size_t _size[128] = {};
    uint8_t _max_track = 0;
    for (typename std::vector<_MidiEvent>::const_iterator it = this->begin(); it != this->end(); ++it) {
        ++_size[it->track];
        _max_track = std::max(it->track, _max_track);
    }
    MidiEventMap<_MidiEvent> ret(_max_track + 1);
    for (typename std::vector<_MidiEvent>::const_iterator it = this->begin(); it != this->end(); ++it) {
        if (_size[it->track]) {
            ret[it->track].reserve(_size[it->track]);
            _size[it->track] = 0;
        }
    }
    for (typename std::vector<_MidiEvent>::const_iterator it = this->begin(); it != this->end(); ++it) {
        ret[it->track].emplace_back(*it);
    }
    return ret;
}


}  // namespace GoldType::MidiParse
#endif
