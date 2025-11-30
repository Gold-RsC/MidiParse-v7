/******************************
 * 播放控制
 * 1. 按下 Ctrl+Space 暂停/继续
 * 2. 按下 Shift+Space 停止播放
 * 3. 按下 Ctrl+Up 速度 +0.1
 * 4. 按下 Ctrl+Down 速度 -0.1
 * 5. 按下 Ctrl+Left 时间 -15s
 * 6. 按下 Ctrl+Right 时间 +15s
 ******************************/

#include"MidiPlayer.hpp"
#include<iostream>
using namespace GoldType::MidiParse;
// 打印时间
void print_time(uint64_t microsecond){
    uint64_t second=microsecond/1000000;
    uint64_t minute=second/60;
    minute%=60;
    second%=60;
    microsecond%=1000000;
    //00:00:00.00
    printf("%02llu:%02llu:%.2f\n",minute,second,microsecond/100000.0);
}
// 播放
void play() {
    // 读取文件名
    std::string filename;
    std::cout<<"filename:../../../midi/";
    std::getline(std::cin,filename);
    if(std::cin.fail()){
        std::cout<<"input error"<<std::endl;
        return;
    }
    std::cin.sync();

    // 初始化播放器
    MidiPlayer player("../../../midi/"+filename);

    // 初始化按键状态
    bool isCtrlSpacePressed = false;
    bool isShiftSpacePressed = false;
    bool isUpPressed = false;
    bool isDownPressed = false;
    bool isLeftPressed = false;
    bool isRightPressed = false;

    // 开始播放
    // player.start_loop();// 循环播放
    player.start_normal();// 正常播放

    while(true){
        // 获取当前时间和速度
        system("cls");
        uint64_t currentTime=player.get_time();
        print_time(currentTime);
        printf("speed:%.2f\n",player.get_speed());
        
        // 播放结束即跳出循环
        if(player.is_stopped()) {
            break;
        }

        // 按下Ctrl+Space暂停/继续
        if((GetAsyncKeyState(VK_SPACE) & 0x8000)&& 
           (GetAsyncKeyState(VK_CONTROL) & 0x8000)) {
            if(!isCtrlSpacePressed) {
                if(player.is_playing()) {
                    player.pause();
                }
                else if(player.is_paused()) {
                    player.play();
                }
                isCtrlSpacePressed = true;
            }
        }
        else {
            isCtrlSpacePressed = false;
        }
        
        // 按下Shift+Space停止播放
        if((GetAsyncKeyState(VK_SHIFT) & 0x8000)&&
           (GetAsyncKeyState(VK_SPACE) & 0x8000)) {
            if(!isShiftSpacePressed){
                player.stop();
                isShiftSpacePressed = true;
                break;
            }
        }
        else{
            isShiftSpacePressed = false;
        }

        // 按下Ctrl+Up速度+0.1
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

        // 按下Ctrl+Down速度-0.1
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

        // 按下Ctrl+Left时间-15s
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

        // 按下Ctrl+Right时间+15s
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

        // 简单延迟
        Sleep(50);
    }
    player.join();
    Sleep(500);
}
int main(){
    play();
    return 0;
}
