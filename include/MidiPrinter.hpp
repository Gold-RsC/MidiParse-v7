/********************************************************************************************************
 * File Name    : MidiPrinter.hpp
 * Author       : Csrua / Gold_RsC
 * github       : https://github.com/Gold-RsC
 * bilibili     : https://space.bilibili.com/361846321
 * QQ           : 310106329
 * Email        : 310106329@qq.com
 * Create Date  : 2020/07/26
 * Update Date  : 2025/12/01
 * Details      : README.md 
 ********************************************************************************************************/
#ifndef MIDIPRINTER_HPP
#define MIDIPRINTER_HPP

#include"MidiParser.hpp"
#include"NotePair.hpp"

namespace GoldType{
    namespace MidiParse{
        enum MidiPrintFormat:uint8_t{
            table,
            json
        };
        enum MidiPrintJsonFormat:uint8_t{
            jsonFormat_minimal=0x01,
            jsonFormat_pretty=0x02,
            jsonFormat_object=0x04,
            jsonFormat_file=0x08,
            jsonFormat_default=jsonFormat_pretty|jsonFormat_object
        };
        
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

        static const char*indentStr_tab="\t";
        static const char*indentStr_space="    ";

        class MidiPrinter{
            private:
                FILE*m_file;
                MidiPrintFormat m_format;
                MidiPrintJsonFormat m_jsonFormat;
                FormatChar m_formatChar[2];
            public:
                MidiPrinter(void);
                MidiPrinter(std::string&&_filename);
                MidiPrinter(const MidiPrinter&)=delete;
                MidiPrinter(MidiPrinter&&_other)=delete;
                ~MidiPrinter(void);
            private:
                void print_table_without_lable(const Note&a,FormatChar formatChar=FormatChar::tableChar_default);
                void print_table_without_lable(const NotePair&a,FormatChar formatChar=FormatChar::tableChar_default);
                void print_table_without_lable(const Tempo&a,FormatChar formatChar=FormatChar::tableChar_default);
                void print_table_without_lable(const BarBeat&a,FormatChar formatChar=FormatChar::tableChar_default);
                void print_table_without_lable(const Text&a,FormatChar formatChar=FormatChar::tableChar_default);
                void print_table_without_lable(const MidiEvent&a,FormatChar formatChar=FormatChar::tableChar_default);

                template<typename _MidiEvent>
                void print_table_without_lable(const MidiEventList<_MidiEvent>&_list,FormatChar formatChar){
                    for(const auto&it:_list){
                        print_table_without_lable(it,formatChar);
                    }
                }
                template<typename _MidiEvent>
                void print_table_without_lable(const MidiEventMap<_MidiEvent>&_map,FormatChar formatChar){
                    for(const auto&j:_map){
                        for(const auto&it:j){
                            print_table_without_lable(it,formatChar);
                        }
                    }
                }
                void print_table_without_lable(const MidiParser&a,FormatChar formatChar=FormatChar::tableChar_default);
                void print_lable(const Note&a,FormatChar formatChar=FormatChar::tableChar_default);
                void print_lable(const NotePair&a,FormatChar formatChar=FormatChar::tableChar_default);
                void print_lable(const Tempo&a,FormatChar formatChar=FormatChar::tableChar_default);
                void print_lable(const BarBeat&a,FormatChar formatChar=FormatChar::tableChar_default);
                void print_lable(const Text&a,FormatChar formatChar=FormatChar::tableChar_default);
                void print_lable(const MidiEvent&a,FormatChar formatChar=FormatChar::tableChar_default);
                
                template<typename _MidiEvent>
                void print_lable(const MidiEventList<_MidiEvent>&_list,FormatChar formatChar){
                    if(_list.size()){
                        print_lable(_list[0],formatChar);
                    }
                }
                template<typename _MidiEvent>
                void print_lable(const MidiEventMap<_MidiEvent>&_map,FormatChar formatChar){
                    for(const auto&j:_map){
                        if(j.size()){
                            print_lable(j[0],formatChar);
                            break;
                        }
                    }
                }
                void print_lable(const MidiParser&a,FormatChar formatChar=FormatChar::tableChar_default);
                template<typename _MidiObject>
                void print_table(const _MidiObject&_obj,FormatChar formatChar){
                    print_lable(_obj,formatChar);
                    print_table_without_lable(_obj,formatChar);
                }

                void print_json_minimal(const Note&a,size_t index=(size_t)-1,MidiPrintJsonFormat jsonFormat=MidiPrintJsonFormat::jsonFormat_default);
                void print_json_minimal(const NotePair&a,size_t index=(size_t)-1,MidiPrintJsonFormat jsonFormat=MidiPrintJsonFormat::jsonFormat_default);
                void print_json_minimal(const Tempo&a,size_t index=(size_t)-1,MidiPrintJsonFormat jsonFormat=MidiPrintJsonFormat::jsonFormat_default);
                void print_json_minimal(const BarBeat&a,size_t index=(size_t)-1,MidiPrintJsonFormat jsonFormat=MidiPrintJsonFormat::jsonFormat_default);
                void print_json_minimal(const Text&a,size_t index=(size_t)-1,MidiPrintJsonFormat jsonFormat=MidiPrintJsonFormat::jsonFormat_default);
                void print_json_minimal(const MidiEvent&a,size_t index=(size_t)-1,MidiPrintJsonFormat jsonFormat=MidiPrintJsonFormat::jsonFormat_default);
                template<typename _MidiEvent>
                void print_json_minimal(const MidiEventList<_MidiEvent>&_list,size_t index,MidiPrintJsonFormat jsonFormat){
                    if(jsonFormat&MidiPrintJsonFormat::jsonFormat_file){
                        fprintf(m_file,"{");
                    }
                    if(index==(size_t)-1){
                        fprintf(m_file,"\"List\":{");
                    }
                    else{
                        fprintf(m_file,"\"List%zu\":{",index);
                    }
                    if(_list.size()){
                        for(size_t i=0;i<_list.size()-1;++i){
                            print_json_minimal(_list[i],i,MidiPrintJsonFormat(jsonFormat&(~MidiPrintJsonFormat::jsonFormat_file)|MidiPrintJsonFormat::jsonFormat_object));
                            fprintf(m_file,",");
                        }
                        print_json_minimal(_list.back(),_list.size()-1,MidiPrintJsonFormat(jsonFormat&(~MidiPrintJsonFormat::jsonFormat_file)|MidiPrintJsonFormat::jsonFormat_object));
                    }
                    fprintf(m_file,"}");
                    if(jsonFormat&MidiPrintJsonFormat::jsonFormat_file){
                        fprintf(m_file,"}");
                    }
                }
                template<typename _MidiEvent>
                void print_json_minimal(const MidiEventMap<_MidiEvent>&_map,size_t index,MidiPrintJsonFormat jsonFormat){
                    if(jsonFormat&MidiPrintJsonFormat::jsonFormat_file){
                        fprintf(m_file,"{");
                    }
                    if(index==(size_t)-1){
                        fprintf(m_file,"\"Map\":{");
                    }
                    else{
                        fprintf(m_file,"\"Map%zu\":{",index);
                    }
                    if(_map.size()){
                        for(size_t i=0;i<_map.size()-1;++i){
                            print_json_minimal(_map[i],i,MidiPrintJsonFormat(jsonFormat&(~MidiPrintJsonFormat::jsonFormat_file)|MidiPrintJsonFormat::jsonFormat_object));
                            fprintf(m_file,",");
                        }
                        print_json_minimal(_map.back(),_map.size()-1,MidiPrintJsonFormat(jsonFormat&(~MidiPrintJsonFormat::jsonFormat_file)|MidiPrintJsonFormat::jsonFormat_object));
                    }
                    fprintf(m_file,"}");
                    if(jsonFormat&MidiPrintJsonFormat::jsonFormat_file){
                        fprintf(m_file,"}");
                    }
                }
                void print_json_minimal(const MidiParser&_parser,size_t index=(size_t)-1,MidiPrintJsonFormat jsonFormat=MidiPrintJsonFormat::jsonFormat_default);

                template<typename...Args>
                void print_json_pretty_newline(const char*indentStr,size_t indent,const char*_format,Args&&...args){
                    for(size_t i=0;i<indent;++i){
                        fprintf(m_file,indentStr);
                    }
                    fprintf(m_file,_format,std::forward<Args>(args)...);
                    fprintf(m_file,"\n");
                }
                template<typename...Args>
                void print_json_pretty_newline_without_enter(const char*indentStr,size_t indent,const char*_format,Args&&...args){
                    for(size_t i=0;i<indent;++i){
                        fprintf(m_file,indentStr);
                    }
                    fprintf(m_file,_format,std::forward<Args>(args)...);
                }
                void print_json_pretty(const Note&a,size_t index=(size_t)-1,size_t indent=0,FormatChar formatChar=FormatChar::jsonChar_default,MidiPrintJsonFormat jsonFormat=MidiPrintJsonFormat::jsonFormat_default);
                void print_json_pretty(const NotePair&a,size_t index=(size_t)-1,size_t indent=0,FormatChar formatChar=FormatChar::jsonChar_default,MidiPrintJsonFormat jsonFormat=MidiPrintJsonFormat::jsonFormat_default);
                void print_json_pretty(const Tempo&a,size_t index=(size_t)-1,size_t indent=0,FormatChar formatChar=FormatChar::jsonChar_default,MidiPrintJsonFormat jsonFormat=MidiPrintJsonFormat::jsonFormat_default);
                void print_json_pretty(const BarBeat&a,size_t index=(size_t)-1,size_t indent=0,FormatChar formatChar=FormatChar::jsonChar_default,MidiPrintJsonFormat jsonFormat=MidiPrintJsonFormat::jsonFormat_default);
                void print_json_pretty(const Text&a,size_t index=(size_t)-1,size_t indent=0,FormatChar formatChar=FormatChar::jsonChar_default,MidiPrintJsonFormat jsonFormat=MidiPrintJsonFormat::jsonFormat_default);
                void print_json_pretty(const MidiEvent&a,size_t index=(size_t)-1,size_t indent=0,FormatChar formatChar=FormatChar::jsonChar_default,MidiPrintJsonFormat jsonFormat=MidiPrintJsonFormat::jsonFormat_default);
                template<typename _MidiEvent>
                void print_json_pretty(const MidiEventList<_MidiEvent>&_list,size_t index=(size_t)-1,size_t indent=0,FormatChar formatChar=FormatChar::jsonChar_default,MidiPrintJsonFormat jsonFormat=MidiPrintJsonFormat::jsonFormat_default){
                    if(formatChar==FormatChar::formatChar_default){
                        formatChar=FormatChar::jsonChar_default;
                    }
                    const char*indentStr=(formatChar&FormatChar::indentChar_tab?indentStr_tab:indentStr_space);
                    if(jsonFormat&MidiPrintJsonFormat::jsonFormat_file){
                        print_json_pretty_newline(indentStr,indent,"{");
                        ++indent;
                    }
                    if(index!=(size_t)-1){
                        print_json_pretty_newline(indentStr,indent,"\"List%zu\": {",index);
                    }
                    else{
                        print_json_pretty_newline(indentStr,indent,"\"List\": {");
                    }
                    ++indent;
                    if(_list.size()){
                        for(size_t i=0;i<_list.size()-1;++i){
                            print_json_pretty(_list[i],i,indent,FormatChar((formatChar&0x03)|FormatChar::endChar_comma),MidiPrintJsonFormat(jsonFormat&(~MidiPrintJsonFormat::jsonFormat_file)|MidiPrintJsonFormat::jsonFormat_object));
                        }
                        print_json_pretty(_list.back(),_list.size()-1,indent,FormatChar((formatChar&0x03)|FormatChar::endChar_newline),MidiPrintJsonFormat(jsonFormat&(~MidiPrintJsonFormat::jsonFormat_file)|MidiPrintJsonFormat::jsonFormat_object));
                    }
                    --indent;
                    if(formatChar&FormatChar::endChar_comma){
                        print_json_pretty_newline(indentStr,indent,"},");
                    }
                    else{
                        print_json_pretty_newline(indentStr,indent,"}");
                    }
                    if(jsonFormat&MidiPrintJsonFormat::jsonFormat_file){
                        --indent;
                        print_json_pretty_newline(indentStr,indent,"}");
                    }
                }
                template<typename _MidiEvent>
                void print_json_pretty(const MidiEventMap<_MidiEvent>&_map,size_t index=(size_t)-1,size_t indent=0,FormatChar formatChar=FormatChar::jsonChar_default,MidiPrintJsonFormat jsonFormat=MidiPrintJsonFormat::jsonFormat_default){
                    if(formatChar==FormatChar::formatChar_default){
                        formatChar=FormatChar::jsonChar_default;
                    }
                    const char*indentStr=(formatChar&FormatChar::indentChar_tab?indentStr_tab:indentStr_space);
                    if(jsonFormat&MidiPrintJsonFormat::jsonFormat_file){
                        print_json_pretty_newline(indentStr,indent,"{");
                        ++indent;
                    }
                    if(index!=(size_t)-1){
                        print_json_pretty_newline(indentStr,indent,"\"Map%zu\": {",index);
                    }
                    else{
                        print_json_pretty_newline(indentStr,indent,"\"Map\": {");
                    }
                    ++indent;
                    if(_map.size()){
                        for(size_t i=0;i<_map.size()-1;++i){
                            print_json_pretty(_map[i],i,indent,FormatChar((formatChar&0x03)|FormatChar::endChar_comma),MidiPrintJsonFormat(jsonFormat&(~MidiPrintJsonFormat::jsonFormat_file)|MidiPrintJsonFormat::jsonFormat_object));
                        }
                        print_json_pretty(_map.back(),_map.size()-1,indent,FormatChar((formatChar&0x03)|FormatChar::endChar_newline),MidiPrintJsonFormat(jsonFormat&(~MidiPrintJsonFormat::jsonFormat_file)|MidiPrintJsonFormat::jsonFormat_object));
                    }
                    --indent;
                    if(formatChar&FormatChar::endChar_comma){
                        print_json_pretty_newline(indentStr,indent,"},");
                    }
                    else{
                        print_json_pretty_newline(indentStr,indent,"}");
                    }
                    if(jsonFormat&MidiPrintJsonFormat::jsonFormat_file){
                        --indent;
                        fprintf(m_file,"}");
                    }
                }
                void print_json_pretty(const MidiParser&_parser,size_t index=(size_t)-1,size_t indent=0,FormatChar formatChar=FormatChar::jsonChar_default,MidiPrintJsonFormat jsonFormat=MidiPrintJsonFormat::jsonFormat_default);
            public:
                MidiPrinter&operator=(const MidiPrinter&)=delete;
                MidiPrinter&operator=(MidiPrinter&&)=delete;
            public:
                MidiPrinter&operator<<(MidiPrintFormat _format);
                MidiPrinter&operator<<(MidiPrintJsonFormat _jsonFormat);
                MidiPrinter&operator<<(FormatChar _formatChar);
                template<typename _MidiObject>
                MidiPrinter&operator<<(const _MidiObject&_obj){
                    if(m_format==MidiPrintFormat::table){
                        print_table(_obj,m_formatChar[0]);
                    }
                    else if(m_format==MidiPrintFormat::json){
                        if(m_jsonFormat&MidiPrintJsonFormat::jsonFormat_minimal){
                            print_json_minimal(_obj,(size_t)-1,m_jsonFormat);
                        }
                        else if(m_jsonFormat&MidiPrintJsonFormat::jsonFormat_pretty){
                            print_json_pretty(_obj, (size_t)-1, 0, m_formatChar[1],m_jsonFormat);
                        }
                    }
                    return*this;
                }
        };
        
    }
}
#endif