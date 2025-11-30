/********************************************************************************************************
 * File Name    : MidiTrack.hpp
 * Author       : Csrua / Gold_RsC
 * github       : Gold-RsC(https://github.com/Gold-RsC)
 * bilibili     : Csrua(https://space.bilibili.com/361846321)
 * Email        : 310106329@qq.com
 * Create Date  : 2020/04/26
 * Update Date  : 2025/12/01
 * Details      : README.md 
 ********************************************************************************************************/
#ifndef MIDITRACK_HPP
#define MIDITRACK_HPP
#include<vector>
#include"MidiEvent.hpp"
namespace GoldType{
    namespace MidiParse{

        using MidiTrack=MidiEventList<MidiEvent>;
        using MidiTrackList=MidiEventMap<MidiEvent>;
        

    }
}
#endif
