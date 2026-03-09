/********************************************************************************************************
 * File Name    : MidiParser.hpp
 * Author       : Csrua / Gold_RsC
 * github       : Gold-RsC(https://github.com/Gold-RsC)
 * bilibili     : Csrua(https://space.bilibili.com/361846321)
 * Email        : 310106329@qq.com
 * Update Date  : 2026/03/06
 * Details      : README.md
 ********************************************************************************************************/
#ifndef MIDIPARSER_HPP
#define MIDIPARSER_HPP
#include "MidiFile.hpp"
#include "MoreMidiEvent.hpp"
namespace GoldType::MidiParse {

struct __get_tempo_time_fun {
    uint64_t operator()(const Tempo& tempo) const {
        return tempo.time;
    }
    uint64_t operator|(const Tempo& tempo) const {
        return tempo.timeNode;
    }
};
struct __get_tempo_timeNode_fun {
    uint64_t operator()(const Tempo& tempo) const {
        return tempo.timeNode;
    }
    uint64_t operator|(const Tempo& tempo) const {
        return tempo.time;
    }
};
template <typename _MidiEvent>
struct __tick2micro_fun {
    uint16_t _tpqn;
    __tick2micro_fun(uint16_t _tpqn)
        : _tpqn(_tpqn) {
    }
    template <typename _GetTime>
    void operator()(_MidiEvent& _event, const Tempo& _tempo, _GetTime&& _getTime) const {
        _event.timeMode = MidiTimeMode::microsecond;
        _event.time = (_getTime | _tempo) + (_event.time - _getTime(_tempo)) * _tempo.mispqn / _tpqn;
    }
};
template <typename _MidiEvent>
struct __micro2tick_fun {
    uint16_t _tpqn;
    __micro2tick_fun(uint16_t _tpqn)
        : _tpqn(_tpqn) {
    }
    template <typename _GetTime>
    void operator()(_MidiEvent& _event, const Tempo& _tempo, _GetTime&& _getTime) const {
        _event.timeMode = MidiTimeMode::tick;
        _event.time = (_getTime | _tempo) + (_event.time - _getTime(_tempo)) * _tpqn / _tempo.mispqn;
    }
};
class MidiParser {
public:
    MidiTimeMode timeMode;
    MidiHead head;
    TempoMap tempoMap;
    BarBeatMap bbMap;
    NoteMap noteMap;
    TextMap textMap;

protected:
    uint8_t get_metaTrack(uint8_t _track) const {
        if (head.format == 0 || head.format == 0x01) {
            return 0;
        }
        else if (head.format == 0x02) {
            return _track;
        }
        return 0xFF;
    }

    MidiErrorCode parse_tick(const MidiFile& m_midi) {
        uint8_t instruments[16] = {0};
        for (size_t trackIdx = 0; trackIdx < head.ntracks; ++trackIdx) {
            uint64_t time = 0;
            uint8_t metaTrack = get_metaTrack(trackIdx);
            if (metaTrack == 0xFF) {
                return MidiErrorCode::parse_error;
            }

            for (size_t eventIdx = 0; eventIdx < m_midi[trackIdx].size(); ++eventIdx) {
                const MidiEvent& event = m_midi[trackIdx][eventIdx];
                time += event.time;
                uint8_t channel = 0xFF;
                if (event.is_normal()) {
                    channel = event.channel();
                }
                switch (event.type()) {
                    case MidiEventType::note_off:
                    case MidiEventType::note_on: {
                        noteMap[trackIdx].emplace_back(time, MidiTimeMode::tick, trackIdx, channel, event[1],
                                                       (event.type() == MidiEventType::note_on) ? event[2] : 0,
                                                       instruments[channel]);
                        break;
                    }
                    case MidiEventType::key_afterTouch: {
                        break;
                    }
                    case MidiEventType::controller: {
                        break;
                    }
                    case MidiEventType::program: {
                        instruments[channel] = event[1];
                        break;
                    }
                    case MidiEventType::channel_afterTouch: {
                        break;
                    }
                    case MidiEventType::pitchWheel: {
                        break;
                    }
                    case MidiEventType::sysex_begin: {
                        break;
                    }
                    case MidiEventType::sysex_end: {
                        break;
                    }
                    case MidiEventType::meta: {
                        MidiMetaType metaType = MidiMetaType(event[1]);
                        switch (metaType) {
                            case MidiMetaType::track_text:
                            case MidiMetaType::song_copyright:
                            case MidiMetaType::track_name:
                            case MidiMetaType::instrument_name:
                            case MidiMetaType::lyric:
                            case MidiMetaType::marker:
                            case MidiMetaType::start_point:
                            case MidiMetaType::program_name:
                            case MidiMetaType::device_name: {
                                uint32_t _size = 0;
                                size_t beginIdx = 2;
                                for (; beginIdx < 6; ++beginIdx) {
                                    _size <<= 7;
                                    _size |= (event[beginIdx] & 0x7F);
                                    if (event[beginIdx] < 0x80) {
                                        ++beginIdx;
                                        break;
                                    }
                                }

                                textMap[trackIdx].emplace_back(time, MidiTimeMode::tick, trackIdx, metaType,
                                                               std::string(event.message.begin() + beginIdx,
                                                                           event.message.begin() + beginIdx + _size));
                                break;
                            }

                            case MidiMetaType::tempo: {
                                uint32_t mispqn = (event[3] << 16) | (event[4] << 8) | (event[5]);
                                if (tempoMap[metaTrack].size()) {
                                    tempoMap[metaTrack].emplace_back(time, MidiTimeMode::tick, trackIdx, mispqn,
                                                                     tempoMap[metaTrack].back().timeNode +
                                                                         (time - tempoMap[metaTrack].back().time) *
                                                                             tempoMap[metaTrack].back().mispqn /
                                                                             head.tpqn());
                                }
                                else {
                                    tempoMap[metaTrack].emplace_back(time, MidiTimeMode::tick, trackIdx, mispqn,
                                                                     time * 0x07A120u / head.tpqn());
                                }
                                break;
                            }
                            case MidiMetaType::time_signature: {
                                double newBeat, newBar;
                                if (bbMap[metaTrack].size()) {
                                    double deltaBeat;
                                    deltaBeat = (time - bbMap[metaTrack].back().time) *
                                                bbMap[metaTrack].back().denominator / 4.0 / head.tpqn();
                                    newBeat = bbMap[metaTrack].back().beatNode + deltaBeat;
                                    newBar =
                                        bbMap[metaTrack].back().barNode + deltaBeat / bbMap[metaTrack].back().numerator;
                                }
                                else {
                                    newBeat = time * 1.0 / head.tpqn();
                                    newBar = newBeat / 4;
                                }
                                bbMap[metaTrack].emplace_back(time, MidiTimeMode::tick, trackIdx, newBar, newBeat,
                                                              event[3], 1 << event[4], event[5], event[6]);
                                break;
                            }
                            default: {
                                break;
                            }
                        }
                        break;
                    }
                    default: {
                        break;
                    }
                }
            }
        }
        set_bb_sorted(noteMap);
        return MidiErrorCode::no_error;
    }
    MidiErrorCode parse_micro(const MidiFile& m_midi) {
        MidiErrorCode err = parse_tick(m_midi);
        if (err != MidiErrorCode::no_error) {
            return err;
        }
        err = change_timeMode(MidiTimeMode::microsecond);
        return err;
    }

    template <typename _MidiEvent>
    bool is_time_sorted(const MidiEventMap<_MidiEvent>& _map) const {
        for (size_t trackIdx = 0; trackIdx < _map.size(); ++trackIdx) {
            for (size_t eventIdx = 1; eventIdx < _map[trackIdx].size(); ++eventIdx) {
                if (_map[trackIdx][eventIdx].time < _map[trackIdx][eventIdx - 1].time) {
                    return false;
                }
            }
        }
        return true;
    }

    size_t _find_bbIdx_between(uint64_t _time, const BarBeatList& _bbList, size_t _left, size_t _right) const {
        if (_left >= _right) {
            return size_t(-1);
        }
        size_t mid = (_left + _right) / 2;

        if (_bbList[mid].time <= _time) {
            if (mid != _bbList.size() - 1) {
                if (_bbList[mid + 1].time > _time) {
                    return mid;
                }
                else {
                    return _find_bbIdx_between(_time, _bbList, mid + 1, _right);
                }
            }
            else {
                return mid;
            }
        }
        return _find_bbIdx_between(_time, _bbList, _left, mid);
    }
    template <typename _MidiEvent>
    MidiErrorCode set_bb_sorted(MidiEventMap<_MidiEvent>& _map) {
        for (size_t trackIdx = 0; trackIdx < _map.size(); ++trackIdx) {
            size_t bbIdx = 0;
            uint8_t metaTrack = get_metaTrack(trackIdx);
            for (size_t eventIdx = 0; eventIdx < _map[trackIdx].size(); ++eventIdx) {
                _MidiEvent& event = _map[trackIdx][eventIdx];
                while (bbMap[metaTrack][bbIdx].time < event.time) {
                    if (bbIdx < bbMap[metaTrack].size() - 1) {
                        if (bbMap[metaTrack][bbIdx + 1].time <= event.time) {
                            ++bbIdx;
                            continue;
                        }
                    }
                    break;
                }
                if (bbMap[metaTrack][bbIdx].time > event.time) {
                    return MidiErrorCode::change_timeMode;
                }
                double deltaBeat = (event.time - bbMap[metaTrack][bbIdx].time) * bbMap[metaTrack][bbIdx].denominator /
                                   4.0 / head.tpqn();
                double deltaBar = deltaBeat / bbMap[metaTrack][bbIdx].numerator;
                event.bar = bbMap[metaTrack][bbIdx].barNode + deltaBar;
                event.beat = bbMap[metaTrack][bbIdx].beatNode + deltaBeat;
            }
        }
        return MidiErrorCode::no_error;
    }

    template <typename _MidiEvent, typename _Fun>
    MidiErrorCode set_bb_unsorted(MidiEventMap<_MidiEvent>& _map) {
        for (size_t trackIdx = 0; trackIdx < _map.size(); ++trackIdx) {
            uint8_t metaTrack = get_metaTrack(trackIdx);
            for (size_t eventIdx = 0; eventIdx < _map[trackIdx].size(); ++eventIdx) {
                _MidiEvent& event = _map[trackIdx][eventIdx];
                size_t bbIdx = _find_bbIdx_between(event.time, bbMap[metaTrack], 0, bbMap[metaTrack].size());
                if (bbIdx == size_t(-1)) {
                    double deltaBeat = (event.time - bbMap[metaTrack][bbIdx].time) *
                                       bbMap[metaTrack][bbIdx].denominator / 4.0 / head.tpqn();
                    double deltaBar = deltaBeat / bbMap[metaTrack][bbIdx].numerator;
                    event.bar = bbMap[metaTrack][bbIdx].barNode + deltaBar;
                    event.beat = bbMap[metaTrack][bbIdx].beatNode + deltaBeat;
                }
                else {
                    return MidiErrorCode::change_timeMode;
                }
            }
        }
        return MidiErrorCode::no_error;
    }

    template <typename _GetTime>
    size_t _find_tempoIdx_between(uint64_t _time, const TempoList& _tempoList, size_t _left, size_t _right,
                                  _GetTime&& _getTime) const {
        if (_left >= _right) {
            return size_t(-1);
        }
        size_t mid = (_left + _right) / 2;

        if (_getTime(_tempoList[mid]) <= _time) {
            if (mid != _tempoList.size() - 1) {
                if (_getTime(_tempoList[mid + 1]) > _time) {
                    return mid;
                }
                else {
                    return _find_tempoIdx_between(_time, _tempoList, mid + 1, _right, _getTime);
                }
            }
            else {
                return mid;
            }
        }
        return _find_tempoIdx_between(_time, _tempoList, _left, mid, _getTime);
    }

    template <typename _MidiEvent, typename _Fun, typename _GetTime>
    MidiErrorCode change_timeMode_sorted(MidiEventMap<_MidiEvent>& _map, _Fun&& _fun, _GetTime&& _getTime) const {
        for (size_t trackIdx = 0; trackIdx < _map.size(); ++trackIdx) {
            size_t tempoIdx = 0;
            uint8_t metaTrack = get_metaTrack(trackIdx);
            for (size_t eventIdx = 0; eventIdx < _map[trackIdx].size(); ++eventIdx) {
                _MidiEvent& event = _map[trackIdx][eventIdx];
                while (_getTime(tempoMap[metaTrack][tempoIdx]) < event.time) {
                    if (tempoIdx < tempoMap[metaTrack].size() - 1) {
                        if (_getTime(tempoMap[metaTrack][tempoIdx + 1]) <= event.time) {
                            ++tempoIdx;
                            continue;
                        }
                    }
                    break;
                }
                if (_getTime(tempoMap[metaTrack][tempoIdx]) > event.time) {
                    return MidiErrorCode::change_timeMode;
                }
                _fun(event, tempoMap[metaTrack][tempoIdx], _getTime);
            }
        }
        return MidiErrorCode::no_error;
    }

    template <typename _MidiEvent, typename _Fun, typename _GetTime>
    MidiErrorCode change_timeMode_unsorted(MidiEventMap<_MidiEvent>& _map, _Fun&& _fun, _GetTime&& _getTime) const {
        for (size_t trackIdx = 0; trackIdx < _map.size(); ++trackIdx) {
            uint8_t metaTrack = get_metaTrack(trackIdx);
            for (size_t eventIdx = 0; eventIdx < _map[trackIdx].size(); ++eventIdx) {
                _MidiEvent& event = _map[trackIdx][eventIdx];
                size_t tempoIdx =
                    _find_tempoIdx_between(event.time, tempoMap[metaTrack], 0, tempoMap[metaTrack].size(), _getTime);
                if (tempoIdx == size_t(-1)) {
                    _fun(event, tempoMap[metaTrack][tempoIdx], _getTime);
                }
                else {
                    return MidiErrorCode::change_timeMode;
                }
            }
        }
        return MidiErrorCode::no_error;
    }

    template <typename _MidiEvent>
    MidiErrorCode change_timeMode_tick2micro_sorted(MidiEventMap<_MidiEvent>& _map) const {
        if (timeMode == MidiTimeMode::tick) {
            return change_timeMode_sorted(_map, __tick2micro_fun<_MidiEvent>(head.tpqn()), __get_tempo_time_fun());
        }
        return change_timeMode_sorted(_map, __tick2micro_fun<_MidiEvent>(head.tpqn()), __get_tempo_timeNode_fun());
    }

    template <typename _MidiEvent>
    MidiErrorCode change_timeMode_micro2tick_sorted(MidiEventMap<_MidiEvent>& _map) const {
        if (timeMode == MidiTimeMode::microsecond) {
            return change_timeMode_sorted(_map, __micro2tick_fun<_MidiEvent>(head.tpqn()), __get_tempo_time_fun());
        }
        return change_timeMode_sorted(_map, __micro2tick_fun<_MidiEvent>(head.tpqn()), __get_tempo_timeNode_fun());
    }

    template <typename _MidiEvent>
    MidiErrorCode change_timeMode_tick2micro_unsorted(MidiEventMap<_MidiEvent>& _map) const {
        if (timeMode == MidiTimeMode::tick) {
            return change_timeMode_unsorted(_map, __tick2micro_fun<_MidiEvent>(head.tpqn()), __get_tempo_time_fun());
        }
        return change_timeMode_unsorted(_map, __tick2micro_fun<_MidiEvent>(head.tpqn()), __get_tempo_timeNode_fun());
    }

    template <typename _MidiEvent>
    MidiErrorCode change_timeMode_micro2tick_unsorted(MidiEventMap<_MidiEvent>& _map) const {
        if (timeMode == MidiTimeMode::microsecond) {
            return change_timeMode_unsorted(_map, __micro2tick_fun<_MidiEvent>(head.tpqn()), __get_tempo_time_fun());
        }
        return change_timeMode_unsorted(_map, __micro2tick_fun<_MidiEvent>(head.tpqn()), __get_tempo_timeNode_fun());
    }

public:
    MidiParser(void) = delete;
    MidiParser(const MidiParser& _midiParser)
        : head(_midiParser.head),
          timeMode(_midiParser.timeMode) {
        if (head.format == 0x00 || head.format == 0x01) {
            tempoMap.resize(1);
            tempoMap[0] = _midiParser.tempoMap[0];
            bbMap.resize(1);
            bbMap[0] = _midiParser.bbMap[0];
        }
        else if (head.format == 0x02) {
            tempoMap.resize(head.ntracks);
            for (size_t i = 0; i < head.ntracks; ++i) {
                tempoMap[i] = _midiParser.tempoMap[i];
            }
            bbMap.resize(head.ntracks);
            for (size_t i = 0; i < head.ntracks; ++i) {
                bbMap[i] = _midiParser.bbMap[i];
            }
        }
        if (_midiParser.noteMap.size()) {
            noteMap.resize(head.ntracks);
            for (size_t i = 0; i < head.ntracks; ++i) {
                noteMap[i] = _midiParser.noteMap[i];
            }
        }
        if (_midiParser.textMap.size()) {
            textMap.resize(head.ntracks);
            for (size_t i = 0; i < head.ntracks; ++i) {
                textMap[i] = _midiParser.textMap[i];
            }
        }
    }
    MidiParser(MidiParser&& _midiParser)
        : head(std::move(_midiParser.head)),
          timeMode(_midiParser.timeMode),
          tempoMap(std::move(_midiParser.tempoMap)),
          bbMap(std::move(_midiParser.bbMap)),
          noteMap(std::move(_midiParser.noteMap)),
          textMap(std::move(_midiParser.textMap)) {
    }
    MidiParser(const MidiFile& _midi, MidiTimeMode _timeMode = MidiTimeMode::tick)
        : timeMode(MidiTimeMode::tick) {
        if (_midi.is_read_success()) {
            head = _midi.head;
            if (head.format == 0x00 || head.format == 0x01) {
                tempoMap.resize(1);
                bbMap.resize(1);
            }
            else if (head.format == 0x02) {
                tempoMap.resize(head.ntracks);
                bbMap.resize(head.ntracks);
            }
            noteMap.resize(head.ntracks);
            textMap.resize(head.ntracks);

            if (_timeMode == MidiTimeMode::tick) {
                parse_tick(_midi);
            }
            else if (_timeMode == MidiTimeMode::microsecond) {
                parse_micro(_midi);
            }
        }
        else {
        }
    }
    MidiParser(MidiFile&& _midi, MidiTimeMode _timeMode = MidiTimeMode::tick)
        : timeMode(MidiTimeMode::tick) {
        if (_midi.is_untouched()) {
            _midi.read();
        }
        if (_midi.is_read_success()) {
            head = _midi.head;
            if (head.format == 0x00 || head.format == 0x01) {
                tempoMap.resize(1);
                bbMap.resize(1);
            }
            else if (head.format == 0x02) {
                tempoMap.resize(head.ntracks);
                bbMap.resize(head.ntracks);
            }
            noteMap.resize(head.ntracks);
            textMap.resize(head.ntracks);

            if (_timeMode == MidiTimeMode::tick) {
                parse_tick(_midi);
            }
            else if (_timeMode == MidiTimeMode::microsecond) {
                parse_micro(_midi);
            }
        }
        else {
        }
    }
    MidiParser(std::string _filename, MidiTimeMode _timeMode = MidiTimeMode::tick)
        : timeMode(MidiTimeMode::tick) {
        MidiFile _midi(_filename);
        _midi.read();
        *this = MidiParser(_midi, _timeMode);
    }
    ~MidiParser(void) = default;

public:
    template <typename _MidiEvent>
    MidiErrorCode change_timeMode(MidiEventMap<_MidiEvent>& _map,
                                  MidiTimeMode _mode = MidiTimeMode::microsecond) const {
        MidiTimeMode _oldTimeMode = _map.get_timeMode();
        MidiErrorCode err = MidiErrorCode::no_error;
        if (_oldTimeMode == _mode) {
            return MidiErrorCode::no_error;
        }
        if (_oldTimeMode == MidiTimeMode::tick && _mode == MidiTimeMode::microsecond) {
            if (is_time_sorted(_map)) {
                err = change_timeMode_tick2micro_sorted(_map);
            }
            else {
                err = change_timeMode_tick2micro_unsorted(_map);
            }
        }
        else if (_oldTimeMode == MidiTimeMode::microsecond && _mode == MidiTimeMode::tick) {
            if (is_time_sorted(_map)) {
                err = change_timeMode_micro2tick_sorted(_map);
            }
            else {
                err = change_timeMode_micro2tick_unsorted(_map);
            }
        }
        return err;
    }
    MidiErrorCode change_timeMode(MidiTimeMode _mode = MidiTimeMode::microsecond) {
        if (timeMode == _mode) {
            return MidiErrorCode::no_error;
        }
        MidiErrorCode err = MidiErrorCode::no_error;
        bool flag = false;
        if (timeMode == MidiTimeMode::tick && _mode == MidiTimeMode::microsecond) {
            err = change_timeMode_tick2micro_sorted(noteMap);
            if (err != MidiErrorCode::no_error) {
                return err;
            }
            err = change_timeMode_tick2micro_sorted(bbMap);
            if (err != MidiErrorCode::no_error) {
                return err;
            }
            err = change_timeMode_tick2micro_sorted(textMap);
            if (err != MidiErrorCode::no_error) {
                return err;
            }
            flag = true;
        }
        else if (timeMode == MidiTimeMode::microsecond && _mode == MidiTimeMode::tick) {
            err = change_timeMode_micro2tick_sorted(noteMap);
            if (err != MidiErrorCode::no_error) {
                return err;
            }
            err = change_timeMode_micro2tick_sorted(bbMap);
            if (err != MidiErrorCode::no_error) {
                return err;
            }
            err = change_timeMode_micro2tick_sorted(textMap);
            if (err != MidiErrorCode::no_error) {
                return err;
            }
            flag = true;
        }
        if (flag) {
            for (uint8_t trackIdx = 0; trackIdx < tempoMap.size(); ++trackIdx) {
                for (size_t eventIdx = 0; eventIdx < tempoMap[trackIdx].size(); ++eventIdx) {
                    tempoMap[trackIdx][eventIdx].timeMode = _mode;
                    std::swap(tempoMap[trackIdx][eventIdx].time, tempoMap[trackIdx][eventIdx].timeNode);
                }
            }
        }
        timeMode = _mode;
        return err;
    }

public:
    MidiParser& operator=(const MidiParser&) = default;
    MidiParser& operator=(MidiParser&&) = default;
};

}  // namespace GoldType::MidiParse
#endif
