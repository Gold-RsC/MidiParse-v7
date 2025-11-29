/********************************************************************************************************
 * File Name    : MidiTrack.hpp
 * Author       : Csrua / Gold_RsC
 * github       : https://github.com/Gold-RsC
 * bilibili     : https://space.bilibili.com/361846321
 * QQ           : 310106329
 * Email        : 310106329@qq.com
 * Create Date  : 2020/07/26
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
        
        void time_delta2absolute(MidiTrack&track);
        void time_delta2absolute(MidiTrackList&tracks);

    }
}
#endif
