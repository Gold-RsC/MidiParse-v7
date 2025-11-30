#include"MidiPrinter.hpp"
using namespace GoldType::MidiParse;
/********************************************************
 * MidiPrintFormat:
    * json
    * table
 * MidiPrintJsonFormat:
    * jsonFormat_object     // 打印为JSON对象
    * jsonFormat_file       // 打印为JSON文件
    ---------------------
    * jsonFormat_minimal    // 打印为JSON最小格式
    * jsonFormat_pretty     // 打印为人类友好格式
    ---------------------
    * jsonFormat_default=jsonFormat_pretty|jsonFormat_object    // 默认JSON格式

 * FormatChar:              // 通常不调整，即保持默认
    * stepChar_tab          // 步长为tab
    * stepChar_space        // 步长为4个空格
    ---------------------
    * indentChar_tab        // 缩进为tab
    * indentChar_space      // 缩进为4个空格
    ---------------------
    * endChar_newline       // 每行结束为换行符
    * endChar_comma         // 每行结束为逗号和换行符
    ---------------------
    * tableChar_default=stepChar_tab|endChar_newline     // 表格默认格式
    * jsonChar_default=indentChar_space|endChar_comma    // JSON默认格式
    ---------------------
    * formatChar_default    // 默认格式(根据MidiPrintFormat由程序自主选择)
 */
void print_table(const NotePairMap&notePairMap){
    MidiPrinter printer("../../../output/note_pair.txt");
    printer<<MidiPrintFormat::table<<notePairMap;
}
void print_json_minimal(const BarBeatMap&bbMap){
    MidiPrinter printer("../../../output/bar_beat.json");
    printer<<MidiPrintFormat::json<<MidiPrintJsonFormat(jsonFormat_minimal|jsonFormat_file)<<bbMap;
}
void print_json_pretty(const TextMap&textMap){
    MidiPrinter printer("../../../output/text.json");
    printer<<MidiPrintFormat::json<<MidiPrintJsonFormat(jsonFormat_pretty|jsonFormat_file)<<textMap;
}
int main(){
    MidiParser parser("../../../midi/faded.mid",MidiTimeMode::microsecond);
    NoteMap noteMap=parser.noteMap();
    BarBeatMap bbMap=parser.bbMap();
    TextMap textMap=parser.textMap();
    print_table(link_notePair(noteMap));
    print_json_minimal(bbMap);
    print_json_pretty(textMap);
    return 0;
}
