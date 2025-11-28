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

        template<>
        MidiErrorType MidiError::operator()<MidiTrack>(const MidiTrack&track){
            MidiErrorType type=MidiErrorType::no_error;
            for(auto&it:track){
                type=(*this)(it);
                if(type!=MidiErrorType::no_error){
                    break;
                }
            }
            return type;
        }

        template<>
        MidiErrorType MidiError::operator()<MidiTrackList>(const MidiTrackList&tracks){
            MidiErrorType type=MidiErrorType::no_error;
            for(auto&track:tracks){
                type=(*this)(track);
                if(type!=MidiErrorType::no_error){
                    break;
                }
            }
            return type;
        }
    }
}
#endif
