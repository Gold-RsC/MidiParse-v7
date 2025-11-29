#include"MidiPrinter.hpp"

namespace GoldType{
    namespace MidiParse{

        MidiPrinter::MidiPrinter(void):m_file(stdout),m_format(MidiPrintFormat::table),m_jsonFormat(MidiPrintJsonFormat::jsonFormat_default),m_formatChar{FormatChar::tableChar_default,FormatChar::jsonChar_default}{
        }
        MidiPrinter::MidiPrinter(std::string&&_filename):m_format(MidiPrintFormat::table),m_jsonFormat(MidiPrintJsonFormat::jsonFormat_default),m_formatChar{FormatChar::tableChar_default,FormatChar::jsonChar_default}{
            m_file=fopen(_filename.c_str(),"w");
        }
        MidiPrinter::~MidiPrinter(void){
            if(m_file&&m_file!=stdout&&m_file!=stdin&&m_file!=stderr){
                fclose(m_file);
            }
        }
            
        void MidiPrinter::print_table_without_lable(const Note&a,FormatChar formatChar){
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
        void MidiPrinter::print_table_without_lable(const NotePair&a,FormatChar formatChar){
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
        void MidiPrinter::print_table_without_lable(const Tempo&a,FormatChar formatChar){
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
        void MidiPrinter::print_table_without_lable(const BarBeat&a,FormatChar formatChar){
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
        void MidiPrinter::print_table_without_lable(const Text&a,FormatChar formatChar){
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
        void MidiPrinter::print_table_without_lable(const MidiEvent&a,FormatChar formatChar){
            if(formatChar==FormatChar::formatChar_default){
                formatChar=FormatChar::tableChar_default;
            }
            char stepC=(formatChar&FormatChar::stepChar_space?' ':'\t');
            char endC=(formatChar&FormatChar::endChar_comma?',':'\n');
            fprintf(m_file,"%llu%c%2.2X%c",
                a.time,
                stepC,
                a.track,
                stepC
            );
            for(size_t i=1;i<a.message.size();++i){
                fprintf(m_file,"%2.2X%c",a.message[i-1],stepC);
            }
            if(a.message.size()){
                fprintf(m_file,"%2.2X",a.message.back());
            }
            fprintf(m_file,"%c",endC);
        }

        
        void MidiPrinter::print_table_without_lable(const MidiParser&a,FormatChar formatChar){
            printf("err");
        }
        void MidiPrinter::print_lable(const Note&a,FormatChar formatChar){
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
        void MidiPrinter::print_lable(const NotePair&a,FormatChar formatChar){
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
        void MidiPrinter::print_lable(const Tempo&a,FormatChar formatChar){
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
        void MidiPrinter::print_lable(const BarBeat&a,FormatChar formatChar){
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
        void MidiPrinter::print_lable(const Text&a,FormatChar formatChar){
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
        void MidiPrinter::print_lable(const MidiEvent&a,FormatChar formatChar){
            if(formatChar==FormatChar::formatChar_default){
                formatChar=FormatChar::tableChar_default;
            }
            char stepC=(formatChar&FormatChar::stepChar_space?' ':'\t');
            char endC=(formatChar&FormatChar::endChar_comma?',':'\n');
            fprintf(m_file,"%s%ctrack%cmessage%c",
                a.timeMode==MidiTimeMode::tick?"tick":"microsecond",
                stepC,
                stepC,
                endC
            );
        }
        
        
        void MidiPrinter::print_lable(const MidiParser&a,FormatChar formatChar){
            printf("err");
        }
        

        void MidiPrinter::print_json_minimal(const Note&a,size_t index,MidiPrintJsonFormat jsonFormat){
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
        void MidiPrinter::print_json_minimal(const NotePair&a,size_t index,MidiPrintJsonFormat jsonFormat){
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
        void MidiPrinter::print_json_minimal(const Tempo&a,size_t index,MidiPrintJsonFormat jsonFormat){
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
        void MidiPrinter::print_json_minimal(const BarBeat&a,size_t index,MidiPrintJsonFormat jsonFormat){
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
        void MidiPrinter::print_json_minimal(const Text&a,size_t index,MidiPrintJsonFormat jsonFormat){
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
        void MidiPrinter::print_json_minimal(const MidiEvent&a,size_t index,MidiPrintJsonFormat jsonFormat){
            if(jsonFormat&MidiPrintJsonFormat::jsonFormat_file){
                fprintf(m_file,"{");
            }
            if(index==(size_t)-1){
                fprintf(m_file,"\"MidiEvent\":");
            }
            else{
                fprintf(m_file,"\"MidiEvent%zu\":",index);
            }
            fprintf(m_file,"{\"track\":\"0x%2.2X\",\"%s\":%llu,\"message\":[",
                a.track,
                a.timeMode==MidiTimeMode::tick?"tick":"microsecond",
                a.time
            );
            for(size_t i=1;i<a.message.size();++i){
                fprintf(m_file,"\"0x%2.2X\",",a.message[i-1]);
            }
            if(a.message.size()){
                fprintf(m_file,"\"0x%2.2X\"",a.message.back());
            }
            fprintf(m_file,"]}");
            if(jsonFormat&MidiPrintJsonFormat::jsonFormat_file){
                fprintf(m_file,"}");
            }
        }
        
        void MidiPrinter::print_json_minimal(const MidiParser&_parser,size_t index,MidiPrintJsonFormat jsonFormat){
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

        void MidiPrinter::print_json_pretty(const Note&a,size_t index,size_t indent,FormatChar formatChar,MidiPrintJsonFormat jsonFormat){
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
        void MidiPrinter::print_json_pretty(const NotePair&a,size_t index,size_t indent,FormatChar formatChar,MidiPrintJsonFormat jsonFormat){
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
        void MidiPrinter::print_json_pretty(const Tempo&a,size_t index,size_t indent,FormatChar formatChar,MidiPrintJsonFormat jsonFormat){
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
        void MidiPrinter::print_json_pretty(const BarBeat&a,size_t index,size_t indent,FormatChar formatChar,MidiPrintJsonFormat jsonFormat){
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
        void MidiPrinter::print_json_pretty(const Text&a,size_t index,size_t indent,FormatChar formatChar,MidiPrintJsonFormat jsonFormat){
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
        void MidiPrinter::print_json_pretty(const MidiEvent&a,size_t index,size_t indent,FormatChar formatChar,MidiPrintJsonFormat jsonFormat){
            if(formatChar==FormatChar::formatChar_default){
                formatChar=FormatChar::jsonChar_default;
            }
            const char*indentStr=(formatChar&FormatChar::indentChar_tab?indentStr_tab:indentStr_space);
            if(jsonFormat&MidiPrintJsonFormat::jsonFormat_file){
                print_json_pretty_newline(indentStr,indent,"{");
                ++indent;
            }
            if(index!=(size_t)-1){
                print_json_pretty_newline(indentStr,indent,"\"MidiEvent%zu\": {",index);
            }
            else{
                print_json_pretty_newline(indentStr,indent,"\"MidiEvent\": {");
            }
            ++indent;
            print_json_pretty_newline(indentStr,indent,"\"track\":\"0x%2.2X\",",a.track);
            print_json_pretty_newline(indentStr,indent,"\"%s\":%llu,",a.timeMode==MidiTimeMode::tick?"tick":"microsecond",a.time);
            print_json_pretty_newline_without_enter(indentStr,indent,"\"message\":[");
            for(size_t i=1;i<a.message.size();++i){
                fprintf(m_file,"\"0x%2.2X\",",a.message[i-1]);
            }
            if(a.message.size()){
                fprintf(m_file,"\"0x%2.2X\"",a.message.back());
            }
            fprintf(m_file,"]\n");
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
        
        
        void MidiPrinter::print_json_pretty(const MidiParser&_parser,size_t index,size_t indent,FormatChar formatChar,MidiPrintJsonFormat jsonFormat){
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
    
        MidiPrinter&MidiPrinter::operator<<(MidiPrintFormat _format){
            m_format=_format;
            return*this;
        }
        MidiPrinter&MidiPrinter::operator<<(MidiPrintJsonFormat _jsonFormat){
            m_jsonFormat=_jsonFormat;
            return*this;
        }
        MidiPrinter&MidiPrinter::operator<<(FormatChar _formatChar){
            if(m_format==MidiPrintFormat::table){
                m_formatChar[0]=_formatChar;
            }
            else if(m_format==MidiPrintFormat::json){
                m_formatChar[1]=_formatChar;
            }
            return*this;
        }
        
        
    }
}