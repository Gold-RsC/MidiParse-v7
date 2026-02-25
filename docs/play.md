# 音乐播放

## 重要说明
- `MidiPlayer`头文件使用`winapi`的`winmm`库进行音乐播放，所以**不兼容非`windows`系统**
- 由于`winapi`的限制，同一时间只允许存在一个句柄播放音乐，也就是说同一时间**只允许存在一个`MidiPlayer`对象**，用户无法通过创建多个`MidiPlayer`对象实现多音乐的同时播放
- `MidiPlayer`头文件的设计初衷是为了方便用户调试时使用，所以它的设计并不追求完美，甚至有些地方是非常粗糙的，但它绝对能满足大多数用户的需求，并且使用起来非常简单，当然如果你有更好的设计方案，也欢迎提交pull request

## 头文件

MidiPlayer.hpp

## 包含内容

### ShortMessage (class)

```c++
class MidiShortMessage{
    public:
        uint64_t time;
        uint32_t message;
    public:
        MidiShortMessage(uint64_t _time=0,uint32_t _message=0);
        MidiShortMessage(uint64_t _time,const MidiMessage&_message);
        MidiShortMessage(const MidiEvent&_event);
};
```

一般情况`ShortMessage`的细节是无需用户关心的，除非用户真的有使用short message的需求



### ShortMessageList (class)

```c++
class MidiShortMessageList:public std::vector<MidiShortMessage>{
    public:
        using std::vector<MidiShortMessage>::vector;
        MidiShortMessageList(const MidiFile&_file);
        MidiShortMessageList(const MidiParser&_parser);
        MidiShortMessageList(const std::string&_filename);
        MidiShortMessageList(NoteList_noteList);
        MidiShortMessageList(const NoteMap&_map);
    public:
        MidiTimeMode get_timeMode(void)const;
};
```

一般情况`ShortMessageList`的细节是无需用户关心的，除非用户真的有使用short message的需求



### MidiPlayer (class)

```c++
class MidiPlayer{
    public:
        MidiPlayer(void);
        template<typename _Object>
        MidiPlayer(_Object&&_object);
        MidiPlayer(const MidiPlayer&);
        MidiPlayer(MidiPlayer&&);
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
        MidiPlayer&operator=(const MidiPlayer&);
        MidiPlayer&operator=(MidiPlayer&&);
};
```

这一部分是`MidiPlayer`头文件的重要内容



#### 构造函数

##### MidiPlayer(void)

```c++
MidiPlayer(void)
```

创建一个空的`MidiPlayer`，在操作前**必须**用拷贝赋值运算/移动赋值运算为`MidiPlayer`配置内容

```c++
//examples\play\game.cpp
#include"MidiPlayer.hpp"

......

using namespace GoldType::MidiParse;

MidiPlayer player;

......

void play() {
    player=MidiPlayer("../../../midi/nggyu.mid");
    player.start_loop();
}

void game() {
    ......
}

int main(){
    play();

    game();

    player.join();
    return 0;
}
```

其中`player=MidiPlayer("../../../midi/nggyu.mid");`确保了`player`在执行操作类函数前不为空。



##### MidiPlayer(_Object&&_object)

```c++
template<typename _Object>
MidiPlayer(_Object&&_object)
```

`_Obejct`的类型可以为以下几种

- `NoteList`

- `NoteMap`

- `NotePairList`

- `NotePairMap`

- `MidiFile`

- `std::string`(filename)

- `MidiParser`
  
  

##### MidiPlayer(const MidiPlayer&)

##### MidiPlayer(MidiPlayer&&)

以上两个函数功能基本相同，均只能复制`MidiPlayer`中的`ShortMessageList`、音乐播放时长、迭代器，而无法复制线程



#### 拷贝/移动赋值函数

用法同拷贝/移动构造函数，只能配合空构造使用





#### 线程类函数-开始播放

这部分有两个版本的播放函数`void start_normal(void)`和`void start_loop(void)`，请注意不要连续调用两次此类函数，否则会导致`std::thread`被移动赋值两次

##### void start_normal(void)

此函数会生成一个播放即结束的线程

##### void start_loop(void)

此函数会生成一个永不结束的线程



#### 线程类函数-join函数、析构函数

##### void join(void)、析构函数

若希望在某处等待播放器结束播放，则需要调用`join(void)`函数

需要注意的是，为防止程序员出错，我在析构函数添加了`join(void)`函数

尽管如此，手动调用`join(void)`是一个好习惯

如下面的例子中，展示了不调用`join(void)`的危害

> 详见[/examples/play/play_error.cpp](../examples/play/play_error.cpp)

由于析构函数中有`join(void)`函数，所以可以实现音效的播放：

```c++
MidiPlayer("sound.mid").start_normal();
```

不过需要注意，临时变量的析构是立即发生的，则会导致程序在执行到这行语句后会等待音乐结束才会执行下面的语句

所以如果希望实现不阻塞的音效播放，则需要用户实现音效池，代码如下：

> 详见[/examples/play/sound_pool.cpp](../examples/play/sound_pool.cpp)



#### 控制类函数

此处的“控制”意为让`MidiPlayer`进行暂停、继续、停止的操作

##### void play(void)

继续播放

##### void pause(void)

暂停播放

##### void stop(void)

停止播放



#### 操作类函数

##### 判断状态

- `bool is_started(void)`是否已经开始

- `bool is_playing(void)`是否处于播放状态

- `bool is_paused(void)`是否处于暂停状态

- `bool is_stopped(void)`是否播放结束

##### 设置/获取数据

- `void set_time(uint64_t time)`

- `uint64_t get_time(void)`

- `void set_speed(double speed)`

- `double get_speed(void)`

需要注意的是，`set_time`和`set_speed`的参数须在有效范围内才有效




