/********************************************************************************************************
 * File Name    : MidiPlayer.hpp
 * Author       : Csrua / Gold_RsC
 * github       : https://github.com/Gold-RsC
 * bilibili     : https://space.bilibili.com/361846321
 * QQ           : 310106329
 * Email        : 310106329@qq.com
 * Create Date  : 2020/07/26
 * Update Date  : 2025/12/01
 * Details      : README.md 
 ********************************************************************************************************/
#ifndef MIDIPLAYER_HPP
#define MIDIPLAYER_HPP
#include<thread>
#include<mutex>
#include<windows.h>
#include<algorithm>
#include<condition_variable>
#pragma comment(lib,"winmm.lib")
#include"MidiParser.hpp"
namespace GoldType{
    namespace MidiParse{
        class MidiShortMessage{
            public:
                uint64_t time;
                uint32_t message;
            public:
                MidiShortMessage(uint64_t _time=0,uint32_t _message=0);
                
                MidiShortMessage(uint64_t _time,const MidiMessage&_message);
                MidiShortMessage(const MidiEvent&_event);
        };
        bool operator==(const MidiShortMessage&a,const MidiShortMessage&b);
        bool operator!=(const MidiShortMessage&a,const MidiShortMessage&b);
        bool operator<(const MidiShortMessage&a,const MidiShortMessage&b);
        bool operator>(const MidiShortMessage&a,const MidiShortMessage&b);
        bool operator<=(const MidiShortMessage&a,const MidiShortMessage&b);
        bool operator>=(const MidiShortMessage&a,const MidiShortMessage&b);
        class MidiShortMessageList:public std::vector<MidiShortMessage>{
            public:
                using std::vector<MidiShortMessage>::vector;
                MidiShortMessageList(const MidiFile&_file);
                MidiShortMessageList(const MidiParser&_parser);
                MidiShortMessageList(const std::string&_filename);
                MidiShortMessageList(NoteList _noteList);
                MidiShortMessageList(const NoteMap&_map);
            public:
                MidiTimeMode get_timeMode(void)const;
        };
        class MidiPlayer{
            private:
                enum class State{
                    beingstarting,
                    stopped,
                    playing,
                    paused,
                    beingjumping,
                    jumping
                };
                HMIDIOUT m_handle;
                MidiShortMessageList m_messages;

                std::thread m_thread;
            
                State m_state;
                std::mutex m_mutex;
                std::condition_variable m_condition;
                double m_speed;

                uint64_t m_current_time;
                LARGE_INTEGER m_freq;
                LARGE_INTEGER m_current_node;

                MidiShortMessageList::iterator m_current_iterator;
            private:
                
                void wait_until(LARGE_INTEGER target_node,double speed,LARGE_INTEGER freq);
                void normal_task(void);
                void loop_task(void);
            public:
                MidiPlayer(void);
                template<typename _Object>
                MidiPlayer(_Object&&_object):
                    m_messages(std::forward<_Object>(_object)),
                    m_state(State::beingstarting),
                    m_speed(1.0),
                    m_handle(nullptr),
                    m_current_time(0),
                    m_current_iterator(m_messages.end()){
                }
                MidiPlayer(const MidiPlayer&)=delete;
                MidiPlayer(MidiPlayer&&)=delete;
                ~MidiPlayer(void);
            public:
                void start_normal(void);
                void start_loop(void);

                void play(void);
                void pause(void);
                void stop(void);

                void join(void);

                bool is_started(void);
                bool is_playing(void);
                bool is_paused(void);
                bool is_stopped(void);

                void set_time(uint64_t time);
                uint64_t get_time(void);
                
                void set_speed(double speed);
                double get_speed(void);
            public:
                MidiPlayer&operator=(const MidiPlayer&)=delete;
                MidiPlayer&operator=(MidiPlayer&&other)=delete;
        };
    }
}
#endif
