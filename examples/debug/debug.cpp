// MidiError.hpp文件上方有#define MIDI_DEBUG和#define MIDI_WARNING
// 将注释去掉以开启DEBUG或WARNING
// 开启#define MIDI_DEBUG后，会在运行时打印错误信息
// 开启#define MIDI_WARNING后，会将部分不正规行为视为错误

#include "MidiFile.hpp"
#include <iostream>
using namespace GoldType::MidiParse;
int main(){
    MidiFile midifile("../../../midi/no_existing.mid");
    // 读取MIDI文件,返回值为读取时发生的错误，如文件名等错误
    MidiErrorType err=midifile.read();

    // 打印错误信息
    midiError<<err;


    // 检查读取到的MIDI文件，返回值为检查出的错误，不包括文件名、标识符之类的错误
    // 下面两个语句等价
    // 若不开启#define MIDI_DEBUG，则为获取错误信息
    // 否则为获取并打印错误信息
    err=midiError(midifile);
    err=midifile.get_error();

    // 打印错误信息
    midiError<<err;

    return 0;
}
