#ifndef MIDITRACK_HPP
#define MIDITRACK_HPP
#include<vector>
#include"MidiEvent.hpp"
namespace GoldType{
    namespace MidiParse{
        /********************************
                    Midi Track
        ********************************/
        using MidiTrack=MidiEventList<MidiEvent>;
        using MidiTrackList=MidiEventMap<MidiEvent>;
        
        void time_delta2absolute(MidiTrack&track){
            uint64_t time=0;
            for(auto&it:track){
                time+=it.time;
                it.time=time;
            }
        }
        void time_delta2absolute(MidiTrackList&tracks){
            for(auto&track:tracks){
                time_delta2absolute(track);
            }
        }
    }
}
#endif
