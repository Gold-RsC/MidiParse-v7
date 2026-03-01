# Midi打印

## 重要说明

- `MidiPrinter`头文件可将部分`Midi`事件打印成可读的字符串，但不支持将`Midi`事件打印成`Midi`文件

## 头文件

MidiPrinter.hpp

## 包含内容

### MidiPrintFormat(enum)

```c++
enum MidiPrintFormat:uint8_t{
    table,
    json,
};
```

该枚举类型指定了输出结果`table表格`还是`json对象`格式

### MidiPrintJsonFormat(enum)

```c++
enum MidiPrintJsonFormat:uint8_t{
    jsonFormat_minimal=0x01,
    jsonFormat_pretty=0x02,
    jsonFormat_object=0x04,
    jsonFormat_file=0x08,
    jsonFormat_default=jsonFormat_pretty|jsonFormat_object
};
```

当输出模式为`json对象`模式时，该枚举类型可进行细化

- 格式相关
  
  - `minimal`: 最小化输出(即没有任何的缩进换行)
  
  - `pretty`: 适合人类阅读的输出模式(有适当的缩进换行)

- 包装格式相关
  
  - `file`: 在外层多套一个`{}`
  
  - `object`: 外层没有`{}`

### FormatChar(enum)

```c++
enum FormatChar:uint8_t{
    formatChar_default=0x00,
    stepChar_tab=0x01,
    stepChar_space=0x02,
    indentChar_tab=0x01,
    indentChar_space=0x02,
    endChar_newline=0x04,
    endChar_comma=0x08,
    tableChar_default=stepChar_tab|endChar_newline,
    jsonChar_default=indentChar_space|endChar_newline
};
```

控制输出格式字符

- `step`: 间隔字符类型(针对`table`格式使用)
  
  * `tab`: 制表符
  
  * `space`: 四个空格

- `indent`: 缩进字符类型(针对`json`格式使用)
  
  - `tab`: 制表符
  
  - `space`: 四个空格

- `end`: 行尾字符类型
  
  - `newline`: 换行
  
  - `comma`: 逗号(加换行)
    
    

### MidiPrinter(class)

```c++
class MidiPrinter{
    public:
        MidiPrinter(void);
        MidiPrinter(std::string&&_filename);
        MidiPrinter(const MidiPrinter&)=delete;
        MidiPrinter(MidiPrinter&&_other)=delete;
        ~MidiPrinter(void);
    public:
        MidiPrinter&operator=(const MidiPrinter&)=delete;
        MidiPrinter&operator=(MidiPrinter&&)=delete;
    public:
        MidiPrinter&operator<<(MidiPrintFormat _format);
        MidiPrinter&operator<<(MidiPrintJsonFormat _jsonFormat);
        MidiPrinter&operator<<(FormatChar _formatChar);
        template<typename _MidiObject>
        MidiPrinter&operator<<(const _MidiObject&_obj);
};


```

#### 构造函数

##### MidiPrinter(void)

```c++
MidiPrinter(void);
```

默认使用`stdout`进行文件输出

若未对`stdout`进行重定向，则用`MidiPrinter(void)`构造的对象会将信息打印在控制台

##### MidiPrinter(std::string&&_filename)

```c++
MidiPrinter(std::string&&_filename);
```

指定一个文件用来存储打印的信息

需要注意的是，文件的打开方式是`w`，即`写入模式`，不是`追加模式`

#### 更改输出格式

```c++
MidiPrinter&operator<<(MidiPrintFormat _format);
MidiPrinter&operator<<(MidiPrintJsonFormat _jsonFormat);
MidiPrinter&operator<<(FormatChar _formatChar);
```

由于`enum`采用的不是`class enum`类型，所以在使用时需要事先声明或强制类型转换

如

```c++
printer<<MidiPrintFormat::table;
```

```c++
printer<<MidiPrintFormat::json<<MidiPrintJsonFormat(jsonFormat_pretty|jsonFormat_object);
```

默认格式为`table``jsonFormat_pretty|jsonFormat_object``formatChar_default`

#### 输出MidiObject

```c++
template<typename _MidiObject>
MidiPrinter&operator<<(const _MidiObject&_obj);
```

支持的`MidiObject`有:

- `MidiEvent`

- `MoreMidiEvent`中的各种类

- `MidiEventList<template _MidiEvent>`

- `MidiEventMap<template _MidiEvent>`

- `MidiParser`
