#include"MidiParser.hpp"
using namespace GoldType::MidiParse;

// 获取打击乐通道中的音符，并塞入noteList中
// 打击乐通道的数值为9，是第10个通道
int main(){
    // 读取并解析
    MidiParser parser("../../../midi/see_you_again.mid",MidiTimeMode::microsecond);
    // 获取通道9的音符事件
    NoteList noteList;
    for_event(parser.noteMap(),[&](const Note&note){
        if(note.channel==9){
            noteList.push_back(note);
        }
    });
    // 转换为音符对
    NotePairList notePairList=link_notePair(noteList);
    // 打印通道9(打击乐通道)的音符对事件
    FILE*file=fopen("output/get_percussion_note_pair.txt","w");
    if(file==nullptr){
        printf("fopen failed\n");
        return -1;
    }
    fprintf(file,
        "time(us)\tduration(us)\tchannel\tpitch\tinstrument\tvelocity\tbar_begin\tbar_end\tbeat_begin\tbeat_end\n");
    for(const NotePair&notePair:notePairList){
        fprintf(file,
            "%llu\t%llu\t0x%2.2X\t0x%2.2X\t0x%2.2X\t0x%2.2X\t%.3f\t%.3f\t%.3f\t%.3f\n",
            notePair.time,notePair.duration,notePair.channel,notePair.pitch,notePair.instrument,notePair.velocity,notePair.bar,notePair.bar+notePair.bar_diff,notePair.beat,notePair.beat+notePair.beat_diff);
    }
    fclose(file);
    return 0;
}
