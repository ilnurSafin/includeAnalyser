//
// Created by Ильнур on 09.05.2021.
//
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <map>
#include <sstream>

#include "IncludeAnalyser.h"

namespace fs = std::filesystem;

IncludeAnalyser::IncludeAnalyser(std::filesystem::directory_entry sourcePath,
                                 const std::vector<std::filesystem::directory_entry> &libsPaths) {
    findFiles(sourcePath);
    for(const auto& p: libsPaths)
        libs.push_back(p.path());
    for (auto &p : cpps) {
        std::vector<std::string> pastFilenames;
        analyze(p, 0, pastFilenames);
    }
}

std::string IncludeAnalyser::getFileDependeciesTree() const {
    return tree.str();
}

const std::map<std::filesystem::path, int> &IncludeAnalyser::getFileEntryes() const {
    return entryes;
}

void IncludeAnalyser::findFiles(std::filesystem::path path) {
    for(auto& p: fs::recursive_directory_iterator(path)) {
        if(p.path().has_extension() && p.path().extension().string() == ".cpp") {
            cpps.push_back(p);
            entryes[p.path().filename()] = 0;
        }
        else if(p.path().has_extension() && p.path().extension().string() == ".h") {
            headers.push_back(p);
            entryes[p.path().filename()] = 0;
        }
    }
}

void IncludeAnalyser::analyze(std::filesystem::path p, int depth, std::vector<std::string>& pastFilenames) {

    std::ifstream f(p);

    //запись в дерево. "Магическое число 2" - это количество точек
    tree << std::string(depth * 2,'.') << p.filename() << (!f.is_open()? " (!)":"") << std::endl;

    //данного файла нет, либо нет прав доступа к нему (!)
    if(!f.is_open())
        return;

    //проверка/защита: был ли уже этот файл в данной цепочке рекурсии
    if(find(pastFilenames.cbegin(), pastFilenames.cend(), p.filename().string()) != pastFilenames.cend()){
        return;
    }else
        pastFilenames.push_back(p.filename().string());


    bool inOneLineComment = false; //мы в однострочном комментарии
    std::string::iterator oneLineComBegin;

    bool inMultiLienComment = false;//мы в многострочном комментарии
    std::string::iterator multiLineComBegin;

    bool inString = false; //мы в строке :)))))))
    std::string::iterator stringBegin;

    bool inInclude = false; // мы в директиве #include
    bool inQuote = false;   //в директиве и в кавычках
    bool inBrackets = false;//в директиве и угловых скобках
    std::string::iterator includeBegin;

    std::string s;
    std::getline(f,s, '\0'); //копируем текущий обрабатываемый файл в буфер для более удобной работы с ним, здесь не заюзать stl
    f.close();

    std::vector<std::string> quoteIncludes; //include ""
    std::vector<std::string> libIncludes;   //include <>

    for(auto iter = s.begin(); iter != s.end(); iter++)
    {
        if(inOneLineComment){ //ищем конец однострочного комментария
            if(*iter == '\n') {
                inOneLineComment = false;
                iter = s.erase(oneLineComBegin, iter);
            }
        }else if(inMultiLienComment){ //ищем конец многострочного комментария
            if((iter+1) !=s.end() && *iter == '*' && *(iter+1) == '/'){
                inMultiLienComment = false;
                iter = s.erase(multiLineComBegin, iter+2);
                --iter;
            }
        }else if(inString){ //ищем конец строки
            if(*(iter-1) != '\\' && *iter == '\"'){
                inString = false;
                iter = s.erase(stringBegin, iter+1);
                --iter;
            }
        }else if(inInclude){ //ищем имя включаемого файла директивой include и его конец
            if( !inQuote && !inBrackets) {
                if(*iter == '\t' || *iter == ' ')
                    continue;
                else if(*iter == '\"') {
                    inQuote = true;
                    includeBegin = iter + 1;
                    continue;
                }else if(*iter == '<') {
                    inBrackets = true;
                    includeBegin = iter + 1;
                    continue;
                }else
                    throw IncludeAnalyserException("In file: " + p.string() + "\nError: non-space char between \"#include\" and '\"'/'<'" + "\n" + *iter + *(iter+1) + *(iter+2));
            }else{
                if(inQuote && *iter == '\"' || inBrackets && *iter == '>'){
                    std::string includePath = std::string(includeBegin, iter);
                    if(includePath.size() == 0)
                        throw IncludeAnalyserException("In file: " + p.string() + "\nError: empty include");
                    if(inQuote)
                        quoteIncludes.push_back(includePath);
                    else
                        libIncludes.push_back(includePath);
                    inInclude = false;
                    inQuote = false;
                    inBrackets = false;

                }else if(inQuote && (*iter == '<' || *iter == '>'))
                    throw IncludeAnalyserException("In file: " + p.string() + "\nError: After \"#include\" and '\"' has '<'/'>'");
                else if (inBrackets && *iter == '\"')
                    throw IncludeAnalyserException("In file: " + p.string() + "\nError: After \"#include\" and '<' has '\"'");

            }
        }else{
            if((iter+1) !=s.end() && *iter == '/' && *(iter+1) == '/') {//поиск начала однострочного комментария
                inOneLineComment = true;
                oneLineComBegin = iter;
            }else if((iter+1) !=s.end() && *iter == '/' && *(iter+1) == '*') {//поиск начала многострочного комментария
                inMultiLienComment = true;
                multiLineComBegin = iter;
            }else if(*iter == '#' && s.find("#include", iter-s.begin()) != std::string::npos && (s.begin() + s.find("#include", iter-s.begin()))==iter && (*(iter+8) == ' ' || *(iter+8) == '\t' || *(iter+8) == '\"' || *(iter+8) == '<')){//поиск начала директивы include !!!!!!!!!!!!!!!!!!!!!!!!!! магические цифры +8 необходимы для проверки, что идёт далее за директивой, ибо, как оказалось, существует например директива #include_next
                inInclude = true;
                iter += 7;
            }else if(*iter == '\"' && (iter == s.begin() || iter != s.begin() && *(iter-1) != '\\')){//поиск начала строки
                inString = true;
                stringBegin = iter;
            }
        }

    }

    //прохо по всем include ""
    for(auto incPathName : quoteIncludes){
        fs::path incFullPath(p.parent_path() / incPathName);
        entryes[incFullPath.filename()] += 1;
        analyze(incFullPath, depth + 1, pastFilenames);
    }
    //прохо по всем include <> и поиск указанных файлов в, переданных аргументом -I, библиотеках
    for(auto libFileName : libIncludes){
        fs::path libFullPath(libFileName);
        for(fs::path libPath : libs){
            libFullPath = libPath / libFileName;
            std::ifstream f(libFullPath);
            if(f.is_open())
                break;
        }
        entryes[libFullPath.filename()] += 1;
        analyze(libFullPath, depth + 1, pastFilenames);
    }
    pastFilenames.pop_back();
}
