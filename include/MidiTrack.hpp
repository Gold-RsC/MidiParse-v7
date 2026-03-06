/********************************************************************************************************
 * File Name    : MidiTrack.hpp
 * Author       : Csrua / Gold_RsC
 * github       : Gold-RsC(https://github.com/Gold-RsC)
 * bilibili     : Csrua(https://space.bilibili.com/361846321)
 * Email        : 310106329@qq.com
 * Update Date  : 2026/03/06
 * Details      : README.md
 ********************************************************************************************************/
#ifndef MIDITRACK_HPP
#define MIDITRACK_HPP
#include "MidiEvent.hpp"
#include <vector>

namespace GoldType::MidiParse {


using MidiTrack = MidiEventList<MidiEvent>;
using MidiTrackList = MidiEventMap<MidiEvent>;


}  // namespace GoldType::MidiParse
#endif
