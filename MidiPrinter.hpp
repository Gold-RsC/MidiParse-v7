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
                MidiPrinter(void):m_file(stdout),m_format(MidiPrintFormat::table),m_jsonFormat(MidiPrintJsonFormat::jsonFormat_default),m_formatChar{FormatChar::tableChar_default,FormatChar::jsonChar_default}{
                }
                MidiPrinter(std::string&&_filename):m_format(MidiPrintFormat::table),m_jsonFormat(MidiPrintJsonFormat::jsonFormat_default),m_formatChar{FormatChar::tableChar_default,FormatChar::jsonChar_default}{
                    m_file=fopen(_filename.c_str(),"w");
                }
                MidiPrinter(const MidiPrinter&)=delete;
                MidiPrinter(MidiPrinter&&_other)=delete;
                ~MidiPrinter(void){
                    if(m_file&&m_file!=stdout&&m_file!=stdin&&m_file!=stderr){
                        fclose(m_file);
                    }
                }
            public:
                void print_table_without_lable(const Note&a,FormatChar formatChar=FormatChar::tableChar_default){
                    if(formatChar==FormatChar::formatChar_default){
                        formatChar=FormatChar::tableChar_default;
                    }
                    char stepC=(formatChar&FormatChar::stepChar_space?' ':'\t');
                    char endC=(formatChar&FormatChar::endChar_comma?',':'\n');
                    fprintf(m_file,"%llu%c0x%2.2X%c0x%2.2X%c0x%2.2X%c0x%2.2X%c0x%2.2X%c%.4f%c%.4f%c",
                        a.time,
                        stepC,
                        a.track,
                        stepC,
                        a.channel,
                        stepC,
                        a.instrument,
                        stepC,
                        a.pitch,
                        stepC,
                        a.velocity,
                        stepC,
                        a.bar,
                        stepC,
                        a.beat,
                        endC
                    );
                    
                }
                void print_table_without_lable(const NotePair&a,FormatChar formatChar=FormatChar::tableChar_default){
                    if(formatChar==FormatChar::formatChar_default){
                        formatChar=FormatChar::tableChar_default;
                    }
                    char stepC=(formatChar&FormatChar::stepChar_space?' ':'\t');
                    char endC=(formatChar&FormatChar::endChar_comma?',':'\n');
                    fprintf(m_file,"%llu%c%llu%c0x%2.2X%c0x%2.2X%c0x%2.2X%c0x%2.2X%c0x%2.2X%c%.4f%c%.4f%c%.4f%c%.4f%c",
                        a.time,
                        stepC,
                        a.duration,
                        stepC,
                        a.track,
                        stepC,
                        a.channel,
                        stepC,
                        a.instrument,
                        stepC,
                        a.pitch,
                        stepC,
                        a.velocity,
                        stepC,
                        a.bar,
                        stepC,
                        a.bar_diff,
                        stepC,
                        a.beat,
                        stepC,  
                        a.beat_diff,
                        endC
                    );
                }
                void print_table_without_lable(const Tempo&a,FormatChar formatChar=FormatChar::tableChar_default){
                    if(formatChar==FormatChar::formatChar_default){
                        formatChar=FormatChar::tableChar_default;
                    }
                    char stepC=(formatChar&FormatChar::stepChar_space?' ':'\t');
                    char endC=(formatChar&FormatChar::endChar_comma?',':'\n');
                    fprintf(m_file,"%llu%c%llu%c%lu%c%.4f%c",
                        a.tick(),
                        stepC,
                        a.microsecond(),
                        stepC,
                        a.mispqn,
                        stepC,
                        a.bpm(),
                        endC
                    );
                }
                void print_table_without_lable(const BarBeat&a,FormatChar formatChar=FormatChar::tableChar_default){
                    if(formatChar==FormatChar::formatChar_default){
                        formatChar=FormatChar::tableChar_default;
                    }
                    char stepC=(formatChar&FormatChar::stepChar_space?' ':'\t');
                    char endC=(formatChar&FormatChar::endChar_comma?',':'\n');
                    fprintf(m_file,"%llu%c%.4f%c%.4f%c%llu%c%llu%c",
                        a.time,
                        stepC,
                        a.barNode,
                        stepC,
                        a.beatNode,
                        stepC,
                        a.numerator,
                        stepC,
                        a.denominator,
                        endC
                    );
                }
                void print_table_without_lable(const Text&a,FormatChar formatChar=FormatChar::tableChar_default){
                    if(formatChar==FormatChar::formatChar_default){
                        formatChar=FormatChar::tableChar_default;
                    }
                    char stepC=(formatChar&FormatChar::stepChar_space?' ':'\t');
                    char endC=(formatChar&FormatChar::endChar_comma?',':'\n');
                    fprintf(m_file,"%llu%c%2.2X%c\"%s\"%c",
                        a.time,
                        stepC,
                        a.type,
                        stepC,
                        a.text.c_str(),
                        endC
                    );
                }

                template<typename _MidiEvent>
                void print_table_without_lable(const MidiEventList<_MidiEvent>&_list,FormatChar formatChar=FormatChar::tableChar_default){
                    for(const auto&it:_list){
                        print_table_without_lable(it,formatChar);
                    }
                }
                template<typename _MidiEvent>
                void print_table_without_lable(const MidiEventMap<_MidiEvent>&_map,FormatChar formatChar=FormatChar::tableChar_default){
                    for(const auto&j:_map){
                        for(const auto&it:j){
                            print_table_without_lable(it,formatChar);
                        }
                    }
                }
                void print_table_without_lable(const MidiParser&a,FormatChar formatChar=FormatChar::tableChar_default){
                    printf("err");
                }
                void print_lable(const Note&a,FormatChar formatChar=FormatChar::tableChar_default){
                    if(formatChar==FormatChar::formatChar_default){
                        formatChar=FormatChar::tableChar_default;
                    }
                    char stepC=(formatChar&FormatChar::stepChar_space?' ':'\t');
                    char endC=(formatChar&FormatChar::endChar_comma?',':'\n');
                    fprintf(m_file,"%s%ctrack%cchannel%cinstrument%cpitch%cvelocity%cbar%cbeat%c",
                        a.timeMode==MidiTimeMode::tick?"tick":"microsecond",
                        stepC,
                        stepC,
                        stepC,
                        stepC,
                        stepC,
                        stepC,
                        stepC,
                        endC
                    );
                }
                void print_lable(const NotePair&a,FormatChar formatChar=FormatChar::tableChar_default){
                    if(formatChar==FormatChar::formatChar_default){
                        formatChar=FormatChar::tableChar_default;
                    }
                    char stepC=(formatChar&FormatChar::stepChar_space?' ':'\t');
                    char endC=(formatChar&FormatChar::endChar_comma?',':'\n');
                    fprintf(m_file,"%s%cduration%ctrack%cchannel%cinstrument%cpitch%cvelocity%cbar%cbeat%cbar_diff%cbeat_diff%c",
                        a.timeMode==MidiTimeMode::tick?"tick":"microsecond",
                        stepC,
                        stepC,
                        stepC,
                        stepC,
                        stepC,
                        stepC,
                        stepC,
                        stepC,
                        stepC,
                        stepC,
                        endC
                    );
                }
                void print_lable(const Tempo&a,FormatChar formatChar=FormatChar::tableChar_default){
                    if(formatChar==FormatChar::formatChar_default){
                        formatChar=FormatChar::tableChar_default;
                    }
                    char stepC=(formatChar&FormatChar::stepChar_space?' ':'\t');
                    char endC=(formatChar&FormatChar::endChar_comma?',':'\n');
                    fprintf(m_file,"tick%cmicrosecond%cmicrosecond_per_quarter_note%cbpm%c",
                        stepC,
                        stepC,
                        stepC,
                        endC
                    );
                }
                void print_lable(const BarBeat&a,FormatChar formatChar=FormatChar::tableChar_default){
                    if(formatChar==FormatChar::formatChar_default){
                        formatChar=FormatChar::tableChar_default;
                    }
                    char stepC=(formatChar&FormatChar::stepChar_space?' ':'\t');
                    char endC=(formatChar&FormatChar::endChar_comma?',':'\n');
                    fprintf(m_file,"%s%cbar%cbeat%cnumerator%cdenominator%c",
                        a.timeMode==MidiTimeMode::tick?"tick":"microsecond",
                        stepC,
                        stepC,
                        stepC,
                        stepC,
                        endC
                    );
                }
                void print_lable(const Text&a,FormatChar formatChar=FormatChar::tableChar_default){
                    if(formatChar==FormatChar::formatChar_default){
                        formatChar=FormatChar::tableChar_default;
                    }
                    char stepC=(formatChar&FormatChar::stepChar_space?' ':'\t');
                    char endC=(formatChar&FormatChar::endChar_comma?',':'\n');
                    fprintf(m_file,"%s%ctype%ctext%c",
                        a.timeMode==MidiTimeMode::tick?"tick":"microsecond",
                        stepC,
                        stepC,
                        endC
                    );
                }
                
                template<typename _MidiEvent>
                void print_lable(const MidiEventList<_MidiEvent>&_list,FormatChar formatChar=FormatChar::tableChar_default){
                    if(_list.size()){
                        print_lable(_list[0],formatChar);
                    }
                }
                template<typename _MidiEvent>
                void print_lable(const MidiEventMap<_MidiEvent>&_map,FormatChar formatChar=FormatChar::tableChar_default){
                    for(const auto&j:_map){
                        if(j.size()){
                            print_lable(j[0],formatChar);
                            break;
                        }
                    }
                }
                void print_lable(const MidiParser&a,FormatChar formatChar=FormatChar::tableChar_default){
                    printf("err");
                }
                template<typename _MidiObject>
                void print_table(const _MidiObject&_obj,FormatChar formatChar=FormatChar::tableChar_default){
                    print_lable(_obj,formatChar);
                    print_table_without_lable(_obj,formatChar);
                }

                void print_json_minimal(const Note&a,size_t index=(size_t)-1,MidiPrintJsonFormat jsonFormat=MidiPrintJsonFormat::jsonFormat_default){
                    if(jsonFormat&MidiPrintJsonFormat::jsonFormat_file){
                        fprintf(m_file,"{");
                    }
                    if(index==(size_t)-1){
                        fprintf(m_file,"\"Note\":");
                    }
                    else{
                        fprintf(m_file,"\"Note%zu\":",index);
                    }
                    fprintf(m_file,"{\"track\":\"0x%2.2X\",\"channel\":\"0x%2.2X\",\"%s\":%llu,\"instrument\":\"0x%2.2X\",\"pitch\":\"0x%2.2X\",\"velocity\":\"0x%2.2X\",\"bar\":%.4f,\"beat\":%.4f}",
                        a.track,
                        a.channel,
                        a.timeMode==MidiTimeMode::tick?"tick":"microsecond",
                        a.time,
                        a.instrument,
                        a.pitch,
                        a.velocity,
                        a.bar,
                        a.beat
                    );
                    if(jsonFormat&MidiPrintJsonFormat::jsonFormat_file){
                        fprintf(m_file,"}");
                    }
                }
                void print_json_minimal(const NotePair&a,size_t index=(size_t)-1,MidiPrintJsonFormat jsonFormat=MidiPrintJsonFormat::jsonFormat_default){
                    if(jsonFormat&MidiPrintJsonFormat::jsonFormat_file){
                        fprintf(m_file,"{");
                    }
                    if(index==(size_t)-1){
                        fprintf(m_file,"\"NotePair\":");
                    }
                    else{
                        fprintf(m_file,"\"NotePair%zu\":",index);
                    }
                    fprintf(m_file,"{\"track\":\"0x%2.2X\",\"channel\":\"0x%2.2X\",\"%s\":%llu,\"duration\":%llu,\"instrument\":\"0x%2.2X\",\"pitch\":\"0x%2.2X\",\"velocity\":\"0x%2.2X\",\"bar\":%.4f,\"bar_diff\":%.4f,\"beat\":%.4f,\"beat_diff\":%.4f}",
                        a.track,
                        a.channel,
                        a.timeMode==MidiTimeMode::tick?"tick":"microsecond",
                        a.time,
                        a.duration,
                        a.instrument,
                        a.pitch,
                        a.velocity,
                        a.bar,
                        a.bar_diff,
                        a.beat,
                        a.beat_diff
                    );
                    if(jsonFormat&MidiPrintJsonFormat::jsonFormat_file){
                        fprintf(m_file,"}");
                    }
                }
                void print_json_minimal(const Tempo&a,size_t index=(size_t)-1,MidiPrintJsonFormat jsonFormat=MidiPrintJsonFormat::jsonFormat_default){
                    if(jsonFormat&MidiPrintJsonFormat::jsonFormat_file){
                        fprintf(m_file,"{");
                    }
                    if(index==(size_t)-1){
                        fprintf(m_file,"\"Tempo\":");
                    }
                    else{
                        fprintf(m_file,"\"Tempo%zu\":",index);
                    }
                    fprintf(m_file,"{\"track\":\"0x%2.2X\",\"tick\":%llu,\"microsecond\":%llu,\"microsecond_per_quarter_note\":%lu,\"bpm\":%.4f}",
                        a.track,
                        a.tick(),
                        a.microsecond(),
                        a.mispqn,
                        a.bpm()
                    );
                    if(jsonFormat&MidiPrintJsonFormat::jsonFormat_file){
                        fprintf(m_file,"}");
                    }
                }
                void print_json_minimal(const BarBeat&a,size_t index=(size_t)-1,MidiPrintJsonFormat jsonFormat=MidiPrintJsonFormat::jsonFormat_default){
                    if(jsonFormat&MidiPrintJsonFormat::jsonFormat_file){
                        fprintf(m_file,"{");
                    }
                    if(index==(size_t)-1){
                        fprintf(m_file,"\"BarBeat\":");
                    }
                    else{
                        fprintf(m_file,"\"BarBeat%zu\":",index);
                    }
                    fprintf(m_file,"{\"track\":\"0x%2.2X\",\"%s\":%llu,\"bar\":%llu,\"beat\":%llu,\"numerator\":%llu,\"denominator\":%llu}",
                        a.track,
                        a.timeMode==MidiTimeMode::tick?"tick":"microsecond",
                        a.time,
                        a.barNode,
                        a.beatNode,
                        a.numerator,
                        a.denominator
                    );
                }
                void print_json_minimal(const Text&a,size_t index=(size_t)-1,MidiPrintJsonFormat jsonFormat=MidiPrintJsonFormat::jsonFormat_default){
                    if(jsonFormat&MidiPrintJsonFormat::jsonFormat_file){
                        fprintf(m_file,"{");
                    }
                    if(index==(size_t)-1){
                        fprintf(m_file,"\"Text\":");
                    }
                    else{
                        fprintf(m_file,"\"Text%zu\":",index);
                    }
                    fprintf(m_file,"{\"track\":\"0x%2.2X\",\"%s\":%llu,\"type\":\"0x%2.2X\",\"text\":\"%s\"}",
                        a.track,
                        a.timeMode==MidiTimeMode::tick?"tick":"microsecond",
                        a.time,
                        a.type,
                        a.text.c_str()
                    );
                    if(jsonFormat&MidiPrintJsonFormat::jsonFormat_file){
                        fprintf(m_file,"}");
                    }
                }
                template<typename _MidiEvent>
                void print_json_minimal(const MidiEventList<_MidiEvent>&_list,size_t index=(size_t)-1,MidiPrintJsonFormat jsonFormat=MidiPrintJsonFormat::jsonFormat_default){
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
                void print_json_minimal(const MidiEventMap<_MidiEvent>&_map,size_t index=(size_t)-1,MidiPrintJsonFormat jsonFormat=MidiPrintJsonFormat::jsonFormat_default){
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
                void print_json_minimal(const MidiParser&_parser,size_t index=(size_t)-1,MidiPrintJsonFormat jsonFormat=MidiPrintJsonFormat::jsonFormat_default){
                    if(jsonFormat&MidiPrintJsonFormat::jsonFormat_file){
                        fprintf(m_file,"{");
                    }
                    if(index==(size_t)-1){
                        fprintf(m_file,"\"Parser\":{");
                    }
                    else{
                        fprintf(m_file,"\"Parser%zu\":{",index);
                    }
                    print_json_minimal(_parser.noteMap(),0,MidiPrintJsonFormat(jsonFormat&(~MidiPrintJsonFormat::jsonFormat_file)|MidiPrintJsonFormat::jsonFormat_object));
                    fprintf(m_file,",");
                    print_json_minimal(_parser.tempoMap(),1,MidiPrintJsonFormat(jsonFormat&(~MidiPrintJsonFormat::jsonFormat_file)|MidiPrintJsonFormat::jsonFormat_object));
                    fprintf(m_file,",");
                    print_json_minimal(_parser.bbMap(),2,MidiPrintJsonFormat(jsonFormat&(~MidiPrintJsonFormat::jsonFormat_file)|MidiPrintJsonFormat::jsonFormat_object));
                    fprintf(m_file,",");
                    print_json_minimal(_parser.textMap(),3,MidiPrintJsonFormat(jsonFormat&(~MidiPrintJsonFormat::jsonFormat_file)|MidiPrintJsonFormat::jsonFormat_object));
                    if(jsonFormat&MidiPrintJsonFormat::jsonFormat_file){
                        fprintf(m_file,"}");
                    }
                }

                template<typename...Args>
                void print_json_pretty_newline(const char*indentStr,size_t indent,const char*_format,Args&&...args){
                    for(size_t i=0;i<indent;++i){
                        fprintf(m_file,indentStr);
                    }
                    fprintf(m_file,_format,std::forward<Args>(args)...);
                    fprintf(m_file,"\n");
                }
                void print_json_pretty(const Note&a,size_t index=(size_t)-1,size_t indent=0,FormatChar formatChar=FormatChar::jsonChar_default,MidiPrintJsonFormat jsonFormat=MidiPrintJsonFormat::jsonFormat_default){
                    if(formatChar==FormatChar::formatChar_default){
                        formatChar=FormatChar::jsonChar_default;
                    }
                    const char*indentStr=(formatChar&FormatChar::indentChar_tab?indentStr_tab:indentStr_space);
                    if(jsonFormat&MidiPrintJsonFormat::jsonFormat_file){
                        print_json_pretty_newline(indentStr,indent,"{");
                        ++indent;
                    }
                    if(index!=(size_t)-1){
                        print_json_pretty_newline(indentStr,indent,"\"Note%zu\": {",index);
                    }
                    else{
                        print_json_pretty_newline(indentStr,indent,"\"Note\": {");
                    }
                    ++indent;
                    print_json_pretty_newline(indentStr,indent,"\"track\":\"0x%2.2X\",",a.track);
                    print_json_pretty_newline(indentStr,indent,"\"channel\":\"0x%2.2X\",",a.channel);
                    print_json_pretty_newline(indentStr,indent,"\"%s\":%llu,",a.timeMode==MidiTimeMode::tick?"tick":"microsecond",a.time);
                    print_json_pretty_newline(indentStr,indent,"\"instrument\":\"0x%2.2X\",",a.instrument);
                    print_json_pretty_newline(indentStr,indent,"\"pitch\":\"0x%2.2X\",",a.pitch);
                    print_json_pretty_newline(indentStr,indent,"\"velocity\":\"0x%2.2X\",",a.velocity);
                    print_json_pretty_newline(indentStr,indent,"\"bar\":%.4f,",a.bar);
                    print_json_pretty_newline(indentStr,indent,"\"beat\":%.4f",a.beat);
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
                void print_json_pretty(const NotePair&a,size_t index=(size_t)-1,size_t indent=0,FormatChar formatChar=FormatChar::jsonChar_default,MidiPrintJsonFormat jsonFormat=MidiPrintJsonFormat::jsonFormat_default){
                    if(formatChar==FormatChar::formatChar_default){
                        formatChar=FormatChar::jsonChar_default;
                    }
                    const char*indentStr=(formatChar&FormatChar::indentChar_tab?indentStr_tab:indentStr_space);
                    if(jsonFormat&MidiPrintJsonFormat::jsonFormat_file){
                        print_json_pretty_newline(indentStr,indent,"{");
                        ++indent;
                    }
                    if(index!=(size_t)-1){
                        print_json_pretty_newline(indentStr,indent,"\"NotePair%zu\": {",index);
                    }
                    else{
                        print_json_pretty_newline(indentStr,indent,"\"NotePair\": {");
                    }
                    ++indent;
                    print_json_pretty_newline(indentStr,indent,"\"track\":\"0x%2.2X\",",a.track);
                    print_json_pretty_newline(indentStr,indent,"\"channel\":\"0x%2.2X\",",a.channel);
                    print_json_pretty_newline(indentStr,indent,"\"%s\":%llu,",a.timeMode==MidiTimeMode::tick?"tick":"microsecond",a.time);
                    print_json_pretty_newline(indentStr,indent,"\"duration\":%llu,",a.duration);
                    print_json_pretty_newline(indentStr,indent,"\"instrument\":\"0x%2.2X\",",a.instrument);
                    print_json_pretty_newline(indentStr,indent,"\"pitch\":\"0x%2.2X\",",a.pitch);
                    print_json_pretty_newline(indentStr,indent,"\"velocity\":\"0x%2.2X\",",a.velocity);
                    print_json_pretty_newline(indentStr,indent,"\"bar\":%.4f,",a.bar);
                    print_json_pretty_newline(indentStr,indent,"\"bar_diff\":%.4f,",a.bar_diff);
                    print_json_pretty_newline(indentStr,indent,"\"beat\":%.4f,",a.beat);
                    print_json_pretty_newline(indentStr,indent,"\"beat_diff\":%.4f",a.beat_diff);
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
                void print_json_pretty(const Tempo&a,size_t index=(size_t)-1,size_t indent=0,FormatChar formatChar=FormatChar::jsonChar_default,MidiPrintJsonFormat jsonFormat=MidiPrintJsonFormat::jsonFormat_default){
                    if(formatChar==FormatChar::formatChar_default){
                        formatChar=FormatChar::jsonChar_default;
                    }
                    const char*indentStr=(formatChar&FormatChar::indentChar_tab?indentStr_tab:indentStr_space);
                    if(jsonFormat&MidiPrintJsonFormat::jsonFormat_file){
                        print_json_pretty_newline(indentStr,indent,"{");
                        ++indent;
                    }
                    if(index!=(size_t)-1){
                        print_json_pretty_newline(indentStr,indent,"\"Tempo%zu\": {",index);
                    }
                    else{
                        print_json_pretty_newline(indentStr,indent,"\"Tempo\": {");
                    }
                    ++indent;
                    print_json_pretty_newline(indentStr,indent,"\"track\":\"0x%2.2X\",",a.track);
                    print_json_pretty_newline(indentStr,indent,"\"tick\":%llu,",a.tick());
                    print_json_pretty_newline(indentStr,indent,"\"microsecond\":%llu,",a.microsecond());
                    print_json_pretty_newline(indentStr,indent,"\"microsecond_per_quarter_note\":%lu,",a.mispqn);
                    print_json_pretty_newline(indentStr,indent,"\"bpm\":%.4f",a.bpm());
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
                void print_json_pretty(const BarBeat&a,size_t index=(size_t)-1,size_t indent=0,FormatChar formatChar=FormatChar::jsonChar_default,MidiPrintJsonFormat jsonFormat=MidiPrintJsonFormat::jsonFormat_default){
                    if(formatChar==FormatChar::formatChar_default){
                        formatChar=FormatChar::jsonChar_default;
                    }
                    const char*indentStr=(formatChar&FormatChar::indentChar_tab?indentStr_tab:indentStr_space);
                    if(jsonFormat&MidiPrintJsonFormat::jsonFormat_file){
                        print_json_pretty_newline(indentStr,indent,"{");
                        ++indent;
                    }
                    if(index!=(size_t)-1){
                        print_json_pretty_newline(indentStr,indent,"\"BarBeat%zu\": {",index);
                    }
                    else{
                        print_json_pretty_newline(indentStr,indent,"\"BarBeat\": {");
                    }
                    ++indent;
                    print_json_pretty_newline(indentStr,indent,"\"track\":\"0x%2.2X\",",a.track);
                    print_json_pretty_newline(indentStr,indent,"\"%s\":%llu,",a.timeMode==MidiTimeMode::tick?"tick":"microsecond",a.time);
                    print_json_pretty_newline(indentStr,indent,"\"bar\":\"%.4f\",",a.barNode);
                    print_json_pretty_newline(indentStr,indent,"\"beat\":\"%.4f\",",a.beatNode);
                    print_json_pretty_newline(indentStr,indent,"\"numerator\":%llu,",a.numerator);
                    print_json_pretty_newline(indentStr,indent,"\"denominator\":%llu",a.denominator);
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
                void print_json_pretty(const Text&a,size_t index=(size_t)-1,size_t indent=0,FormatChar formatChar=FormatChar::jsonChar_default,MidiPrintJsonFormat jsonFormat=MidiPrintJsonFormat::jsonFormat_default){
                    if(formatChar==FormatChar::formatChar_default){
                        formatChar=FormatChar::jsonChar_default;
                    }
                    const char*indentStr=(formatChar&FormatChar::indentChar_tab?indentStr_tab:indentStr_space);
                    if(jsonFormat&MidiPrintJsonFormat::jsonFormat_file){
                        print_json_pretty_newline(indentStr,indent,"{");
                        ++indent;
                    }
                    if(index!=(size_t)-1){
                        print_json_pretty_newline(indentStr,indent,"\"Text%zu\": {",index);
                    }
                    else{
                        print_json_pretty_newline(indentStr,indent,"\"Text\": {");
                    }
                    ++indent;
                    print_json_pretty_newline(indentStr,indent,"\"track\":\"0x%2.2X\",",a.track);
                    print_json_pretty_newline(indentStr,indent,"\"%s\":%llu,",a.timeMode==MidiTimeMode::tick?"tick":"microsecond",a.time);
                    print_json_pretty_newline(indentStr,indent,"\"type\":\"0x%2.2X\",",a.type);
                    print_json_pretty_newline(indentStr,indent,"\"text\":\"%s\"",a.text.c_str());
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
                void print_json_pretty(const MidiParser&_parser,size_t index=(size_t)-1,size_t indent=0,FormatChar formatChar=FormatChar::jsonChar_default,MidiPrintJsonFormat jsonFormat=MidiPrintJsonFormat::jsonFormat_default){
                    if(formatChar==FormatChar::formatChar_default){
                        formatChar=FormatChar::jsonChar_default;
                    }
                    const char*indentStr=(formatChar&FormatChar::indentChar_tab?indentStr_tab:indentStr_space);
                    if(jsonFormat&MidiPrintJsonFormat::jsonFormat_file){
                        print_json_pretty_newline(indentStr,indent,"{");
                        ++indent;
                    }
                    if(index!=(size_t)-1){
                        print_json_pretty_newline(indentStr,indent,"\"MidiParser%zu\": {",index);
                    }
                    else{
                        print_json_pretty_newline(indentStr,indent,"\"MidiParser\": {");
                    }
                    ++indent;
                    print_json_pretty(_parser.noteMap(),0,indent,FormatChar((formatChar&0x03)|FormatChar::endChar_comma),MidiPrintJsonFormat(jsonFormat&(~MidiPrintJsonFormat::jsonFormat_file)|MidiPrintJsonFormat::jsonFormat_object));
                    print_json_pretty(_parser.tempoMap(),1,indent,FormatChar((formatChar&0x03)|FormatChar::endChar_comma),MidiPrintJsonFormat(jsonFormat&(~MidiPrintJsonFormat::jsonFormat_file)|MidiPrintJsonFormat::jsonFormat_object));
                    print_json_pretty(_parser.bbMap(),2,indent,FormatChar((formatChar&0x03)|FormatChar::endChar_comma),MidiPrintJsonFormat(jsonFormat&(~MidiPrintJsonFormat::jsonFormat_file)|MidiPrintJsonFormat::jsonFormat_object));
                    print_json_pretty(_parser.textMap(),3,indent,FormatChar((formatChar&0x03)|FormatChar::endChar_newline),MidiPrintJsonFormat(jsonFormat&(~MidiPrintJsonFormat::jsonFormat_file)|MidiPrintJsonFormat::jsonFormat_object));
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
            public:
                MidiPrinter&operator=(const MidiPrinter&)=delete;
                MidiPrinter&operator=(MidiPrinter&&)=delete;
            public:
                MidiPrinter&operator<<(MidiPrintFormat _format){
                    m_format=_format;
                    return*this;
                }
                MidiPrinter&operator<<(MidiPrintJsonFormat _jsonFormat){
                    m_jsonFormat=_jsonFormat;
                    return*this;
                }
                MidiPrinter&operator<<(FormatChar _formatChar){
                    if(m_format==MidiPrintFormat::table){
                        m_formatChar[0]=_formatChar;
                    }
                    else if(m_format==MidiPrintFormat::json){
                        m_formatChar[1]=_formatChar;
                    }
                    return*this;
                }
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