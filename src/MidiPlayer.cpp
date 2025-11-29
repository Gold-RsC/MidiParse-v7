#include "MidiPlayer.hpp"
namespace GoldType{
    namespace MidiParse{
        
        MidiShortMessage::MidiShortMessage(uint64_t _time,uint32_t _message):time(_time),message(_message){
        }
        
        MidiShortMessage::MidiShortMessage(uint64_t _time,const MidiMessage&_message){
            time=_time;
            switch(_message.type()){
                case MidiEventType::note_off:
                case MidiEventType::note_on:
                case MidiEventType::key_afterTouch:
                case MidiEventType::controller:
                case MidiEventType::pitchWheel:
                {
                    message=_message[0]|_message[1]<<8|_message[2]<<16;
                    break;
                }
                case MidiEventType::program:
                case MidiEventType::channel_afterTouch:{
                    message=_message[0]|_message[1]<<8;
                    break;
                }
                default:{
                    
                }
            }
        }
        MidiShortMessage::MidiShortMessage(const MidiEvent&_event):MidiShortMessage(_event.time,_event.message){
        }
        bool operator==(const MidiShortMessage&a,const MidiShortMessage&b){
            return a.time==b.time&&a.message==b.message;
        }
        bool operator!=(const MidiShortMessage&a,const MidiShortMessage&b){
            return !(a==b);
        }
        bool operator<(const MidiShortMessage&a,const MidiShortMessage&b){
            if(a.time!=b.time){
                return a.time<b.time;
            }
            return a.message<b.message;
        }
        bool operator>(const MidiShortMessage&a,const MidiShortMessage&b){
            if(a.time!=b.time){
                return a.time>b.time;
            }
            return a.message>b.message;
        }
        bool operator<=(const MidiShortMessage&a,const MidiShortMessage&b){
            return !(a>b);
        }
        bool operator>=(const MidiShortMessage&a,const MidiShortMessage&b){
            return !(a<b);
        }
        MidiShortMessageList::MidiShortMessageList(const MidiFile&_file){
            if(_file.is_read_success()){
                MidiParser _parser(_file);
                MidiTrackList tracks=_file.tracks;
                time_delta2absolute(tracks);
                _parser.change_timeMode(tracks,MidiTimeMode::microsecond);
                for_event(tracks,[this](const MidiEvent&event){
                    if(event.is_normal()){
                        emplace_back(event);
                    }
                });
                std::sort(this->begin(),this->end());
            }
        }
        MidiShortMessageList::MidiShortMessageList(const MidiParser&_parser){
            NoteMap noteMap=_parser.noteMap();
            if(_parser.timeMode()==MidiTimeMode::microsecond){
                *this=MidiShortMessageList(noteMap);
            }
            else{
                _parser.change_timeMode(noteMap,MidiTimeMode::microsecond);
                *this=MidiShortMessageList(noteMap);
            }
        }
        MidiShortMessageList::MidiShortMessageList(const std::string&_filename){
            MidiFile file(_filename);
            file.read();
            *this=MidiShortMessageList(file);
        }
        MidiShortMessageList::MidiShortMessageList(NoteList _noteList){
            _noteList.sort();
            this->reserve(_noteList.size()+_noteList.size()/10);
            uint8_t instruments[16]={0};
            for(const Note&note:_noteList){
                if(note.timeMode==MidiTimeMode::tick){
                    continue;
                }
                if(note.instrument!=instruments[note.channel]){
                    instruments[note.channel]=note.instrument;
                    emplace_back(note.time,(note.instrument<<8)|0xC0|note.channel);
                }
                if(note.velocity){
                    emplace_back(note.time,(note.velocity<<16)|(note.pitch<<8)|0x90|note.channel);
                }
                else{
                    emplace_back(note.time,(note.pitch<<8)|0x80|note.channel);
                }
            }
            std::sort(this->begin(),this->end());
        }
        MidiShortMessageList::MidiShortMessageList(const NoteMap&_map):MidiShortMessageList(event_map_to_list(_map)){
        }
    
        MidiTimeMode MidiShortMessageList::get_timeMode(void)const {
            return MidiTimeMode::microsecond;
        }
        void MidiPlayer::wait_until(LARGE_INTEGER target_node,double speed,LARGE_INTEGER freq){
            while(true){
                LARGE_INTEGER pointer;
                QueryPerformanceCounter(&pointer);
                
                if(pointer.QuadPart>=target_node.QuadPart){
                    break;
                }
                uint64_t remainingTime=(target_node.QuadPart-pointer.QuadPart)*1000000llu/freq.QuadPart;
                if(remainingTime>=10000){
                    std::this_thread::sleep_for(std::chrono::milliseconds(remainingTime/1000));
                }
                else if(remainingTime>=100){
                    std::this_thread::sleep_for(std::chrono::microseconds(remainingTime));
                }
                else{
                    std::this_thread::yield();
                }
            }
            
        }
        void MidiPlayer::normal_task(void){
            LARGE_INTEGER freq;
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                midiOutOpen(&m_handle,0,0,0,CALLBACK_NULL);
                midiOutReset(m_handle);
                QueryPerformanceFrequency(&m_freq);
                QueryPerformanceCounter(&m_current_node);
                m_current_iterator=m_messages.begin();
                m_current_time=m_current_iterator->time;
                freq=m_freq;
            }
            for(;;){
                uint64_t target_time;
                double speed;
                
                LARGE_INTEGER target_node;
                {
                    std::unique_lock<std::mutex> lock(m_mutex);
                    if(m_state==State::playing){
                        while(abs(m_speed)<0.01){
                            lock.unlock();
                            pause();
                            lock.lock();
                            midiOutReset(m_handle);
                            LARGE_INTEGER pause_begin;
                            QueryPerformanceCounter(&pause_begin);
                            m_condition.wait(lock,[this]{
                                return m_state!=State::paused;
                            });
                            if(m_state==State::stopped){
                                m_current_iterator=m_messages.end();
                                return;
                            }
                            LARGE_INTEGER pause_end;
                            QueryPerformanceCounter(&pause_end);
                            m_current_node.QuadPart+=pause_end.QuadPart-pause_begin.QuadPart;
                        }
                    }
                    else if(m_state==State::paused){
                        do{
                            if(abs(m_speed)<0.01){
                                lock.unlock();
                                pause();
                                lock.lock();
                            }
                            
                            midiOutReset(m_handle);
                            LARGE_INTEGER pause_begin;
                            QueryPerformanceCounter(&pause_begin);
                            m_condition.wait(lock,[this]{
                                return m_state!=State::paused;
                            });
                            if(m_state==State::stopped){
                                m_current_iterator=m_messages.end();
                                return;
                            }
                            LARGE_INTEGER pause_end;
                            QueryPerformanceCounter(&pause_end);
                            m_current_node.QuadPart+=pause_end.QuadPart-pause_begin.QuadPart;
                        }while(abs(m_speed)<0.01);
                    }
                    else if(m_state==State::stopped){
                        m_current_iterator=m_messages.end();
                        return;
                    }
                    else if(m_state==State::beingjumping){
                        midiOutReset(m_handle);
                        m_state=State::jumping;
                        m_condition.notify_one();
                        m_condition.wait(lock,[this]{
                            return m_state==State::playing;
                        });
                        if(m_state==State::stopped){
                            m_current_iterator=m_messages.end();
                            return;
                        }
                        
                    }
                    if(m_current_iterator==m_messages.end()){
                        break;
                    }
                    speed=m_speed;
                    target_time=m_current_iterator->time;
                    target_node.QuadPart=m_current_node.QuadPart+(target_time-m_current_time)*freq.QuadPart/1000000llu/speed;
                }
                
                wait_until(target_node,speed,freq);
                {
                    std::unique_lock<std::mutex> lock(m_mutex);
                    m_current_time=target_time;
                    m_current_node=target_node;
                    midiOutShortMsg(m_handle,m_current_iterator->message);
                    ++m_current_iterator;
                }

            }
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_state=State::stopped;
            }
        }
        void MidiPlayer::loop_task(void){
            LARGE_INTEGER freq;
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                MMRESULT result=midiOutOpen(&m_handle,0,0,0,CALLBACK_NULL);
                if(result!=MMSYSERR_NOERROR){
                    throw std::runtime_error("midiOutOpen failed");
                }
                midiOutReset(m_handle);
                QueryPerformanceFrequency(&m_freq);
                QueryPerformanceCounter(&m_current_node);
                m_current_iterator=m_messages.begin();
                m_current_time=m_current_iterator->time;
                freq=m_freq;
            }
            for(;;){
                uint64_t target_time;
                double speed;
                
                LARGE_INTEGER target_node;
                {
                    std::unique_lock<std::mutex> lock(m_mutex);
                    
                    if(m_state==State::playing){
                        while(abs(m_speed)<0.01){
                            lock.unlock();
                            pause();
                            lock.lock();
                            midiOutReset(m_handle);
                            LARGE_INTEGER pause_begin;
                            QueryPerformanceCounter(&pause_begin);
                            m_condition.wait(lock,[this]{
                                return m_state!=State::paused;
                            });
                            if(m_state==State::stopped){
                                m_current_iterator=m_messages.end();
                                return;
                            }
                            LARGE_INTEGER pause_end;
                            QueryPerformanceCounter(&pause_end);
                            m_current_node.QuadPart+=pause_end.QuadPart-pause_begin.QuadPart;
                        }
                    }
                    else if(m_state==State::paused){
                        do{
                            if(abs(m_speed)<0.01){
                                lock.unlock();
                                pause();
                                lock.lock();
                            }
                            
                            midiOutReset(m_handle);
                            LARGE_INTEGER pause_begin;
                            QueryPerformanceCounter(&pause_begin);
                            m_condition.wait(lock,[this]{
                                return m_state!=State::paused;
                            });
                            if(m_state==State::stopped){
                                m_current_iterator=m_messages.end();
                                return;
                            }
                            LARGE_INTEGER pause_end;
                            QueryPerformanceCounter(&pause_end);
                            m_current_node.QuadPart+=pause_end.QuadPart-pause_begin.QuadPart;
                        }while(abs(m_speed)<0.01);
                    }
                    else if(m_state==State::stopped){
                        m_current_iterator=m_messages.end();
                        return;
                    }
                    else if(m_state==State::beingjumping){
                        midiOutReset(m_handle);
                        m_state=State::jumping;
                        m_condition.notify_one();
                        m_condition.wait(lock,[this]{
                            return m_state==State::playing;
                        });
                        if(m_state==State::stopped){
                            m_current_iterator=m_messages.end();
                            return;
                        }
                        
                    }
                    if(m_current_iterator==m_messages.end()){
                        m_current_iterator=m_messages.begin();
                        m_current_time=m_current_iterator->time;
                    }
                    speed=m_speed;
                    target_time=m_current_iterator->time;
                    target_node.QuadPart=m_current_node.QuadPart+(target_time-m_current_time)*freq.QuadPart/1000000llu/speed;
                }
                
                wait_until(target_node,speed,freq);
                {
                    std::unique_lock<std::mutex> lock(m_mutex);
                    m_current_time=target_time;
                    m_current_node=target_node;
                    midiOutShortMsg(m_handle,m_current_iterator->message);
                    ++m_current_iterator;
                }

            }
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_state=State::stopped;
            }
        }
    
        MidiPlayer::MidiPlayer(void):
            m_state(State::beingstarting),
            m_speed(1.0),
            m_handle(nullptr),
            m_current_time(0),
            m_current_iterator(m_messages.end()){

        }
        
        MidiPlayer::~MidiPlayer(void){
            if(m_handle!=nullptr){
                midiOutReset(m_handle);
                midiOutClose(m_handle);
                m_handle=nullptr;
            }
            join();
        }
    
        void MidiPlayer::start_normal(void){
            if(m_state==State::beingstarting){
                m_state=State::playing;
                m_thread=std::thread(&MidiPlayer::normal_task,this);
            }
        }
        void MidiPlayer::start_loop(void){
            if(m_state==State::beingstarting){
                m_state=State::playing;
                m_thread=std::thread(&MidiPlayer::loop_task,this);
            }
        }
        void MidiPlayer::play(void){
            std::lock_guard<std::mutex> lock(m_mutex);
            if(m_state==State::paused){
                
                m_state=State::playing;
                m_condition.notify_one();
            }
            else {

            }

        }
        void MidiPlayer::pause(void){
            std::lock_guard<std::mutex> lock(m_mutex);
            
            if(m_state==State::playing){
                
                m_state=State::paused;
            }
            
        }
        void MidiPlayer::stop(void){
            std::lock_guard<std::mutex> lock(m_mutex);
            if(m_state==State::playing){
                m_state=State::stopped;
            }
            else if(m_state==State::paused){
                m_state=State::stopped;
                m_condition.notify_one();
            }
            else{
                m_state=State::stopped;
            }
            
        }
        void MidiPlayer::set_time(uint64_t time){
            std::unique_lock<std::mutex> lock(m_mutex);
            if(m_state==State::jumping||m_state==State::beingjumping){
                return;
            }
            if(m_messages.empty()){
                return;
            }
            if(m_current_iterator->time==time){
                return;
            }

            if(m_state==State::playing){
                m_state=State::beingjumping;
                m_condition.wait(lock,[this]{
                    return m_state==State::jumping;
                });
                if(time>m_messages.back().time){
                    m_current_iterator=m_messages.end()-1;
                }
                else{
                    if(m_current_iterator==m_messages.end()){
                        m_current_iterator=m_messages.begin();
                    }
                    if(m_current_iterator->time<time){
                        for(;m_current_iterator!=m_messages.end();++m_current_iterator){
                            if(m_current_iterator->time>time){
                                break;
                            }
                        }
                        --m_current_iterator;
                    }
                    else if(m_current_iterator->time>time){
                        for(;m_current_iterator!=m_messages.begin();--m_current_iterator){
                            if(m_current_iterator->time<=time){
                                break;
                            }
                        };
                    }
                }
                m_current_time=m_current_iterator->time;
                ++m_current_iterator;
                QueryPerformanceCounter(&m_current_node);
                m_state=State::playing;
                m_condition.notify_one();
            }
            else if(m_state==State::paused){
                if(m_current_iterator==m_messages.end()){
                    m_current_iterator=m_messages.begin();
                }
                if(time>m_messages.back().time){
                    m_current_iterator=m_messages.end()-1;
                }
                else{
                    if(m_current_iterator->time<time){
                        for(;m_current_iterator!=m_messages.end();++m_current_iterator){
                            if(m_current_iterator->time>time){
                                break;
                            }
                        }
                        --m_current_iterator;
                    }
                    else if(m_current_iterator->time>time){
                        for(;m_current_iterator!=m_messages.begin();--m_current_iterator){
                            if(m_current_iterator->time<=time){
                                break;
                            }
                        };
                    }
                }
                m_current_time=m_current_iterator->time;
                ++m_current_iterator;
            }
            else if(m_state==State::stopped){
                lock.unlock();
                play();
                pause();
                set_time(time);
            }
            
            
        }
        void MidiPlayer::join(void){
            if(m_thread.joinable()){
                m_thread.join();
            }
        }
        bool MidiPlayer::is_started(void){
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_state==State::beingstarting;
        }
        bool MidiPlayer::is_playing(void){
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_state==State::playing;
        }
        bool MidiPlayer::is_paused(void){
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_state==State::paused;
        }
        bool MidiPlayer::is_stopped(void){
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_state==State::stopped;
        }
        uint64_t MidiPlayer::get_time(void){
            std::lock_guard<std::mutex> lock(m_mutex);
            if(m_state==State::playing){
                LARGE_INTEGER _current_time;
                QueryPerformanceCounter(&_current_time);
                return (_current_time.QuadPart-m_current_node.QuadPart)*1000000llu*m_speed/m_freq.QuadPart+m_current_time;
            }
            return m_current_time;
        }
        void MidiPlayer::set_speed(double speed){
            if(speed>0){
                std::lock_guard<std::mutex> lock(m_mutex);
                m_speed=speed;
            }
            
            
        }
        double MidiPlayer::get_speed(void){
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_speed;
        }
    }
}