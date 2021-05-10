//
// Created by Ильнур on 09.05.2021.
// Парсер аргументов программы
// Не стал делать отдельный cpp файл, т.к. компилятор всё-равно сделает это за меня и класс маленький
//
#ifndef INCLUDEANALYSER_INPUTPARSER_H
#define INCLUDEANALYSER_INPUTPARSER_H

#include <vector>
#include <string>
#include <exception>
#include <filesystem>

class InputParser{
public:
    InputParser(int argc, char **argv){
        for(std::size_t argIter=1; argIter < argc; argIter++){
            args.push_back(std::string(argv[argIter]));
        }
        if(args.size() == 0){
            throw ArgException("Not provided source path.");
        }
        sourcePath.assign(args[0]);
        if(!sourcePath.is_directory()) {
            throw ArgException("Provided source path is not directory or doesn't exist.");
        }
        int iterLibs = 1;
        for(; iterLibs < args.size(); iterLibs++){
            if(iterLibs % 2 == 1 && args[iterLibs] != "-I" && args[iterLibs] != "-i"){ //перебираем все -I и -i, по логике они нечетные
                throw ArgException("Wrong option flag.");
            }else if(iterLibs % 2 == 0){ //перебираем все директории библеотек, по логике они четные
                libPaths.push_back(std::filesystem::directory_entry(args[iterLibs]));
                if(!libPaths.back().is_directory()) {
                    throw ArgException("Provided lib path \"" + libPaths.back().path().string() + "\" is not directory or doesn't exist.");
                }
            }
        }
        if(iterLibs > 1 && iterLibs % 2 == 0){ //был указан флаг, но за ним не указана дериктория библиотеки
            throw ArgException("Lib path is not provided after -I option.");
        }
    }

    const std::filesystem::directory_entry& getSourcePath()const{
        return sourcePath;
    }

    std::vector<std::filesystem::directory_entry>& getLibPaths(){
        return libPaths;
    }

    class ArgException : public std::exception{
        std::string _msg;
    public:
        ArgException(const std::string& msg) : _msg(msg + "\nLaunch pattern is: analyser <source path> [options]. Where option is: -I <include path>"){}

        virtual const char* what() const noexcept override
        {
            return _msg.c_str();
        }
    };
private:
    std::filesystem::directory_entry sourcePath; //путь к исходникам
    std::vector<std::filesystem::directory_entry> libPaths; //пути к библиотекам, переданным программе в качестве аргументов
    std::vector<std::string> args;
};

#endif //INCLUDEANALYSER_INPUTPARSER_H
