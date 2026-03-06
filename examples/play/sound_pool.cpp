#define MIDIPLAYER_NO_WARNING
#include"MidiPlayer.hpp"
using namespace GoldType::MidiParse;
MidiPlayer* ptr=nullptr;
int main(){
    
    
    while(1){
        system("pause");
        if(ptr){
            ptr->stop();
            delete ptr;
            ptr=nullptr;
        }
        ptr=new MidiPlayer("../../../midi/note_60.mid");
        ptr->start_normal();
        printf("Sound is playing\n");
    }

    
    return 0;
}