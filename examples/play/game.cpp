#include"MidiPlayer.hpp"
#include<iostream>
#include<windows.h>
#include<conio.h>
#include<cstdlib>
#include<ctime>
using namespace GoldType::MidiParse;

MidiPlayer player;

const int SCREEN_WIDTH = 80;
const int SCREEN_HEIGHT = 20;
const int GROUND_Y = 15;
const int DINOSAUR_X = 5;
const int JUMP_HEIGHT = 8;
const int INITIAL_JUMP_SPEED = 3;
const int MAX_OBSTACLE_SPEED = 5;
int obstacleSpeed = 2;

void setCursorPosition(int x, int y) {
    if (x < 0) x = 0;
    if (x >= SCREEN_WIDTH) x = SCREEN_WIDTH - 1;
    if (y < 0) y = 0;
    if (y >= SCREEN_HEIGHT) y = SCREEN_HEIGHT - 1;
    
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}


void play() {
    player = MidiPlayer("../../../midi/nggyu.mid");
    player.start_loop();
}

void game() {
    srand(time(NULL));
    system("cls");
    CONSOLE_CURSOR_INFO cursor_info = {1, 0};
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);

    int dinosaurY = GROUND_Y;
    int jumpState = 0;  // 0: 地面, 1: 上升, 2: 下降
    int jumpProgress = 0;
    int obstacleX = SCREEN_WIDTH;
    int obstacleType = rand() % 2;  // 0: 小障碍物, 1: 大障碍物
    int score = 0;
    bool gameOver = false;
    
    while (!gameOver) {
        // 绘制地面
        setCursorPosition(0, GROUND_Y);
        for (int i = 0; i < SCREEN_WIDTH; i++) {
            std::cout << "_";
        }
        
        // 绘制恐龙 - 确保位置有效
        if (dinosaurY - 1 >= 0) {
            setCursorPosition(DINOSAUR_X, dinosaurY - 1);
            std::cout << "/\\";
        }
        setCursorPosition(DINOSAUR_X, dinosaurY);
        std::cout << "\\/";
        
        // 绘制障碍物
        setCursorPosition(obstacleX, GROUND_Y);
        std::cout << "|";
        if (obstacleType == 1) {
            setCursorPosition(obstacleX, GROUND_Y - 1);
            std::cout << "|";
        }
        
        // 显示分数
        setCursorPosition(SCREEN_WIDTH - 10, 0);
        std::cout << "分数: " << score;
        
        // 显示音乐时间
        long long microseconds = player.get_time();
        int seconds = microseconds / 1000000;
        int minutes = seconds / 60;
        int remainingSeconds = seconds % 60;
        setCursorPosition(7, 0);
        std::cout << "音乐时间坐标: " << minutes << ":" << (remainingSeconds < 10 ? '0' : '\0') << remainingSeconds;

        // 显示音乐速度
        setCursorPosition(8, 2);
        std::cout << "音乐速度: " << player.get_speed() << "x";

        // 显示游戏难度
        setCursorPosition(SCREEN_WIDTH - 10, 2);
        std::cout << "难度: " << obstacleSpeed << "  ";

        // 处理键盘输入
        if (_kbhit()) {
            char key = _getch();
            if (key == ' ' && dinosaurY >= GROUND_Y) {
                jumpState = 1;
                jumpProgress = 0;
            }
            if (key == 'q' || key == 'Q') {
                gameOver = true;
            }
        }
        
        // 保存恐龙的当前位置用于清除
        int prevDinoY = dinosaurY;
        
        // 更新恐龙位置（跳跃逻辑）
        if (jumpState == 1) {  // 上升
            dinosaurY--;
            jumpProgress++;
            if (jumpProgress >= JUMP_HEIGHT || dinosaurY <= 0) {
                jumpState = 2;
                // 确保不会跳出屏幕顶部
                if (dinosaurY <= 0) {
                    dinosaurY = 0;
                }
            }
        } else if (jumpState == 2) {  // 下降
            dinosaurY++;
            jumpProgress--;
            if (dinosaurY >= GROUND_Y) {
                dinosaurY = GROUND_Y;
                jumpState = 0;
            }
        }
        
        // 保存障碍物的当前位置用于清除
        int prevObstacleX = obstacleX;
        
        // 更新障碍物位置
        obstacleX -= obstacleSpeed;
        if (obstacleX < 0) {
            obstacleX = SCREEN_WIDTH;
            obstacleType = rand() % 2;
            score += 10;
            // 随着分数增加，游戏难度提高
            if (score % 50 == 0) {
                if(obstacleSpeed < MAX_OBSTACLE_SPEED){
                    obstacleSpeed += 1;
                }
                else{
                    obstacleSpeed = 2;
                }
                // 同时增加midi播放速度
                player.set_speed(player.get_speed()+0.08);
            }
        }
        
        // 碰撞检测
        if (abs(obstacleX - DINOSAUR_X) < 3) {
            if (obstacleType == 0 && dinosaurY >= GROUND_Y) {
                gameOver = true;
            } else if (obstacleType == 1 && dinosaurY >= GROUND_Y - 1) {
                gameOver = true;
            }
        }
        
        // 控制游戏速度
        Sleep(50);
        
        // 清除上一帧的恐龙 - 更安全的清除方式
        if (prevDinoY - 1 >= 0) {
            setCursorPosition(DINOSAUR_X, prevDinoY - 1);
            std::cout << "  ";
        }
        setCursorPosition(DINOSAUR_X, prevDinoY);
        std::cout << "  ";
        
        // 清除上一帧的障碍物 - 更安全的清除方式
        setCursorPosition(prevObstacleX, GROUND_Y);
        std::cout << " ";
        if (obstacleType == 1) {
            setCursorPosition(prevObstacleX, GROUND_Y - 1);
            std::cout << " ";
        }
    }
    
    // 游戏结束画面
    system("cls");
    setCursorPosition(SCREEN_WIDTH / 2 - 5, SCREEN_HEIGHT / 2);
    std::cout << "游戏结束";
    setCursorPosition(SCREEN_WIDTH / 2 - 10, SCREEN_HEIGHT / 2 + 1);
    std::cout << "最终分数: " << score;
    setCursorPosition(SCREEN_WIDTH / 2 - 15, SCREEN_HEIGHT / 2 + 3);
    system("pause");

    // 停止midi播放
    player.stop();
    
    // 恢复光标显示
    cursor_info = {1, 1};
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}

int main(){
    // 播放midi文件
    play();

    // 同时执行其他任务
    game();

    // 等待线程结束
    player.join();
    return 0;
}