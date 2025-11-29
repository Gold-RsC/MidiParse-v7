#include "MidiTrack.hpp"
namespace GoldType{
    namespace MidiParse{
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

        // MidiErrorType get_error(MidiError&_midiError,const MidiTrack&track){
        //     MidiErrorType type=MidiErrorType::no_error;
        //     for(auto&it:track){
        //         type=get_error(_midiError,it);
        //         if(type!=MidiErrorType::no_error){
        //             break;
        //         }
        //     }
        //     return type;
        // }
        // MidiErrorType get_error(MidiError&_midiError,const MidiTrackList&tracks){
        //     MidiErrorType type=MidiErrorType::no_error;
        //     for(auto&track:tracks){
        //         type=get_error(_midiError,track);
        //         if(type!=MidiErrorType::no_error){
        //             break;
        //         }
        //     }
        //     return type;
        // }
    }
}