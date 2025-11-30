#define MIDI_DEBUG
#include"MidiParser.hpp"
#include"NotePair.hpp"
#include"MidiPrinter.hpp"
#include"MidiPlayer.hpp"
#include<iostream>
#include<algorithm>
using namespace GoldType;
using namespace MidiParse;
using namespace std;
// void fun1(){
//     for(int i=0;i<n;++i){
//         getline(cin,filename[i]);
//     }
//     for(int i=0;i<n;++i){
//         cout<<"This is:"<<filename[i]<<endl;
//         play(filename[i]);
//     }
// }
// void fun2(){
//     for(int i=0;i<n;++i){
//         getline(cin,filename[i]);
//         MidiParse a(filename[i]);
//         printf("format:%d\n",a.midi().format());
//         printf("tick_per_qn:%d\n",a.midi().tpqn());
//         printf("tick time mis_per_qn(6e7/bpm):\n");
//         if(a.midi().format()==2){
//             for(int i=0;i<a.midi().ntracks();++i){
//                 printf("track%d\n",i);
//                 auto c=a.timeMap();
//                 for(auto it:c[i]){
//                     printf("(%llu %llu %d)\n",it.time(),it.timeNode(),it.mispqn());
//                 }
//             }
//         }
//         else if(a.midi().format()==0||a.midi().format()==1){
//             printf("track0\n");
//             auto c=a.timeMap();
//             for(auto it:c[0]){
//                 printf("(%llu %llu %d)\n",it.time(),it.timeNode(),it.mispqn());
//             }
//         }
//         TsList b=a.tsList();
//         printf("tick n d:\n");
//         for(auto&it:b){
//             printf("(%llu %d %d)\n",it.time(),it.numerator(),it.denominator());
//         }
// //        a.change_allTimeState();
//         sort(a.noteList().begin(),a.noteList().end());
//         FILE*f=fopen("testwrite.txt","w");
//         a.write_noteList(f,"@t\t@r\t@c\t@i\t@p\t@v\t@B\t@b");
//         fclose(f);
//     }
// }
// void fun3(){
//     MidiParser a("2.mid",MidiTimeMode::tick);
//     auto c=a.noteMap;
    
//     a.change_timeMode_one(c,MidiTimeMode::microsecond);
//     FILE*f=fopen("write_test.txt","w");
//     fprintf(f,"time\tduration\ttrack\tchannel\tinstrument\tpitch\tvelocity\tbar\tbeat\n");
//     printf("%d %d\n",c.size(),a.ntracks());
//     for(register int i=0;i<a.ntracks();++i){
//         printf("%d,",c[i].size());
//         for(const auto&it:c[i]){
//             fprintf(f,"%llu\t%X\t%X\t%2.2X\t%2.2X\t%2.2X\t%.6f\t%.6f\n",it.time,it.track,it.channel,it.instrument,it.pitch,it.velocity,it.bar,it.beat);
//         }
//     }
    
//     fclose(f);
// }
// void fun4(){
//     MidiParser a("2.mid",MidiTimeMode::tick);
//     //1
//     auto c=event_map_to_list(a.noteMap);

//     auto d=event_list_to_map(c);
//     a.change_timeMode_one(d);
//     c=event_map_to_list(d);

//     //2
//     // auto d=a.noteMap;
//     // a.change_timeMode_one(d);
//     // auto c=event_map_to_list(d);

//     FILE*f=fopen("write_test_fun4.txt","w");
//     fprintf(f,"time\tduration\ttrack\tchannel\tinstrument\tpitch\tvelocity\tbar\tbeat\n");
//     for(const auto&it:c){
//         fprintf(f,"%llu\t%X\t%X\t%2.2X\t%2.2X\t%2.2X\t%.6f\t%.6f\n",it.time,it.track,it.channel,it.instrument,it.pitch,it.velocity,it.bar,it.beat);
//     }
//     fclose(f);
// }
// void fun5(){
//     MidiParser a("2.mid",MidiTimeMode::tick);
//     auto c=a.noteMap;
//     a.change_timeMode_one(c,MidiTimeMode::microsecond);
//     printf("done\n");
//     NotePairMap d=link_notePair(c);
//     printf("done\n");
//     FILE*f=fopen("write_test_fun5.txt","w");
//     fprintf(f,"time\tduration\ttrack\tchannel\tinstrument\tpitch\tvelocity\tbar\tbeat\n");
//     for(const auto&j:d){
//         for(const auto&it:j){
//             fprintf(f,"%llu\t%llu\t%X\t%X\t%2.2X\t%2.2X\t%2.2X\t%.6f\t%.6f\n",it.time,it.duration,it.track,it.channel,it.instrument,it.pitch,it.velocity,it.bar,it.beat);
//         }
//     }
//     fclose(f);
// }
// void fun6(){
//     MidiParser a("2.mid");
//     auto c=a.noteMap;
//     a.change_timeMode_one(c,MidiTimeMode::microsecond);
//     auto b=link_notePair(c);
//     MidiPrinter pout("write_test_fun6.json");

    
//     pout<<MidiPrintFormat::json<<MidiPrintJsonFormat(jsonFormat_pretty|jsonFormat_file)<<b;
    
        
// }
void fun7(){ //²âÊÔ²¥·Å¹¦ÄÜ
    MidiPrinter pout("write_test_fun7.txt");
    // std::string filename;
    // cin>>filename;
    MidiParser a("1.mid");
    // a.change_timeMode(MidiTimeMode::microsecond);
    // midiDebug<<a.change_timeMode();
    // pout<<link_notePair(a.noteMap());
    MidiPlayer player(a);
    player.play();
    
    
    pout<<link_notePair(a.noteMap());
    // pout<<MidiPrintFormat::json<<MidiPrintJsonFormat(jsonFormat_file|jsonFormat_pretty)<<a.noteMap();

    // player.join();
}
void print_time(uint64_t microsecond){
    uint64_t second=microsecond/1000000;
    uint64_t minute=second/60;
    minute%=60;
    second%=60;
    microsecond%=1000000;
    //00:00:00.00
    printf("%02llu:%02llu:%02.2f\n",minute,second,microsecond/100000.0);
}
void fun8() {
    std::string filename;
    cout<<"filename:"<<endl;
    getline(cin,filename);
    MidiPrinter pout("write_test_fun8.json");
    MidiParser parser(filename,MidiTimeMode::microsecond);
    pout<< MidiPrintFormat::json<<MidiPrintJsonFormat(jsonFormat_file|jsonFormat_pretty)<<parser.textMap();
    MidiPlayer player(filename);
    bool isSpacePressed = false;
    bool isF1Pressed = false;
    bool isUpPressed = false;
    bool isDownPressed = false;
    bool isLeftPressed = false;
    bool isRightPressed = false;
    player.start_loop();
    // player.set_speed(1.5);
    while(true){
        uint64_t currentTime=player.get_time();
        system("cls");
        print_time(currentTime);
        printf("speed:%.2f\n",player.get_speed());
        
        if(player.is_stopped()) {
            break;
        }
        if((GetAsyncKeyState(VK_SPACE) & 0x8000)&& 
           (GetAsyncKeyState(VK_CONTROL) & 0x8000)) {
            if(!isSpacePressed) {
                if(player.is_playing()) {
                    player.pause();
                }
                else if(player.is_paused()) {
                    player.play();
                }
                isSpacePressed = true;
            }
        }
        else {
            isSpacePressed = false;
        }
        
        if((GetAsyncKeyState(VK_SHIFT) & 0x8000)&&
           (GetAsyncKeyState(VK_SPACE) & 0x8000)) {
            if(!isF1Pressed){
                player.stop();
                isF1Pressed = true;
                break;
            }
        }
        else{
            isF1Pressed = false;
        }
        if((GetAsyncKeyState(VK_UP) & 0x8000)&&
           (GetAsyncKeyState(VK_CONTROL) & 0x8000)) {
            if(!isUpPressed) {
                player.set_speed(player.get_speed()+0.1);
                isUpPressed = true;
            }
        }
        else {
            isUpPressed = false;
        }
        if((GetAsyncKeyState(VK_DOWN) & 0x8000)&&
           (GetAsyncKeyState(VK_CONTROL) & 0x8000)) {
            if(!isDownPressed) {
                player.set_speed(player.get_speed()-0.1);
                isDownPressed = true;
            }
        }
        else {
            isDownPressed = false;
        }
        if((GetAsyncKeyState(VK_LEFT) & 0x8000)&&
           (GetAsyncKeyState(VK_CONTROL) & 0x8000)) {
            if(!isLeftPressed) {
                player.set_time(currentTime-15000000);
                isLeftPressed = true;
            }
        }
        else {
            isLeftPressed = false;
        }
        if((GetAsyncKeyState(VK_RIGHT) & 0x8000)&&
           (GetAsyncKeyState(VK_CONTROL) & 0x8000)) {
            if(!isRightPressed) {
                player.set_time(currentTime+15000000);
                isRightPressed = true;
            }
        }
        else {
            isRightPressed = false;
        }
        Sleep(50);
    }
    // player.join();
}
void fun9(){
    MidiFile a("1.mid");
    a.read();
    MidiPrinter mout("write_test_fun9.json");
    mout<< MidiPrintFormat::json<<MidiPrintJsonFormat(jsonFormat_file|jsonFormat_pretty)<<a.tracks;
}
int main(){
    fun8();
    return 0;
}
