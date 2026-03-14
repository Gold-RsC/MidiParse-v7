
#include <iostream>
// #define MIDI_DEBUG
#include "MidiFile.hpp"
using namespace GoldType::MidiParse;
int main(void) {
    try {
        std::string filename;
        std::cin >> filename;
        MidiFile midi(filename);
        midi.read();
    }
    catch (const MidiException& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
