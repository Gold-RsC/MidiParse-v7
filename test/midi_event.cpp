#include "MidiFile.hpp"
#include <iostream>
using namespace GoldType::MidiParse;
int main(void) {
    try {
        MidiFile midi("..\\..\\midi\\note_60.mid");
        midi.read();
        size_t s1 = 0, s2 = 0;
        MidiTrack track = midi.tracks.merge_event();
        track.for_event([&](const MidiEvent& event) { ++s1; });
        MidiTrackList tracklist = track.split_event();
        tracklist.for_event([&](const MidiEvent& event) { ++s2; });
        std::cout << "s1 = " << s1 << std::endl;
        std::cout << "s2 = " << s2 << std::endl;
    }
    catch (const MidiException& e) {
        std::cerr << e.what() << std::endl;
    }
    system("pause");
    return 0;
}
