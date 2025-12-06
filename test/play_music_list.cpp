#include <iostream>

#include <vector>
#include <string>
#include<windows.h>
#include"MidiPlayer.hpp"

using namespace GoldType::MidiParse;

//get all mid files in folder
std::vector<std::string> get_files(const std::string& folderPath) {
    std::vector<std::string> files;
    WIN32_FIND_DATAA findFileData;
    HANDLE hFind = FindFirstFileA((LPCSTR)((folderPath + "/*.mid").c_str()), &findFileData);
    if (hFind != INVALID_HANDLE_VALUE) {
        do{
            if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                files.push_back(folderPath + "\\" + (char*)findFileData.cFileName);
            }
        } while (FindNextFileA(hFind, &findFileData));
        FindClose(hFind);
        
    }
    else{
        std::cerr << "错误: 无法访问文件夹 " << folderPath << " - " << GetLastError() << std::endl;
    }
    return files;
}

// 使用示例
int main(int argc, char* argv[]) {
    if(argc!=2){
        std::cerr << "错误: 请输入一个文件夹路径" << std::endl;
        return 1;
    }
    std::string midiFolder = argv[1];  // 相对于当前目录的路径
    std::cout<<"输入的文件夹路径为: "<<midiFolder<<std::endl;
    // std::cin>>midiFolder;
    // 或者使用绝对路径: std::string midiFolder = "e:\\github_repository\\MidiParse-v7\\midi";
    
    std::vector<std::string> files = get_files(midiFolder);
    
    std::cout << "找到 " << files.size() << " 个mid文件:" << std::endl;
    
    for (const auto& file : files) {
        std::cout<<file<<std::endl;
        MidiFile midi(file);
        std::cout<<"读取中"<<std::endl;
        midi.read();
        std::cout<<"读取完成"<<std::endl;
        if(!midi.is_read_success()){
            std::cout << "错误: 无法读取文件 " << file<< std::endl;
            continue;
        }
        std::cout<<"准备播放"<<std::endl;
        MidiPlayer player(midi);
        std::cout<<"播放中"<<std::endl;
        player.start_normal();
        bool space_pressed = false;
        while(!player.is_stopped()){
            if((GetAsyncKeyState(VK_SPACE) & 0x8000)&&((GetAsyncKeyState(VK_CONTROL) & 0x8000))){
                if(!space_pressed){
                    player.stop();
                    space_pressed = true;
                }
            }
            else{
                if(space_pressed){
                    space_pressed = false;
                }
            }
            Sleep(50);
        }
        player.join();
    }
    
    return 0;
}