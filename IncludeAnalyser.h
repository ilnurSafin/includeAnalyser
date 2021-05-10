//
// Created by Ильнур on 09.05.2021.
// Анализатор файлов исходного кода на предмет директивы #include
//

#ifndef INCLUDEANALYSER_INCLUDEANALYSER_H
#define INCLUDEANALYSER_INCLUDEANALYSER_H

#include <string>
#include <vector>
#include <filesystem>
#include <map>

class IncludeAnalyser {
public:
    IncludeAnalyser(std::filesystem::directory_entry sourcePath, const std::vector<std::filesystem::directory_entry>& libsPaths);
    IncludeAnalyser(const IncludeAnalyser& another) = delete;
    IncludeAnalyser& operator=(const IncludeAnalyser& another) = delete;

    //метод возвращающий дерево зависимостей
    std::string getFileDependeciesTree()const;
    //метод возвращающий список файлов с их частотами
    const std::map<std::filesystem::path, int>& getFileEntryes()const;

    class IncludeAnalyserException : public std::exception{
        std::string _msg;
    public:
        IncludeAnalyserException(const std::string& msg) : _msg(msg){}

        virtual const char* what() const noexcept override
        {
            return _msg.c_str();
        }
    };
protected:
    //метод выполняет поиск и сохранение всех .cpp и .h файлов в source-path
    void findFiles(std::filesystem::path path);
    /*рекурсивный метод поиска всех директив "#include" в том числе и вложенных, за исключением тех, которые находятся в строках и комментрариях
     * p - директория, в которой нужно выполнить рекурсивный обход
     * depth - глубина, на которую мы вошли (нужна для вывода точек в дереве)
     * pastFilenames - имена файлов, в которые мы заходим по мере глубины рекурсии (необходима во избежания бесконечного вложения, в т.ч. бесконечной рекурсии)
     * */
    void analyze(std::filesystem::path p, int depth, std::vector<std::string> &pastFilenames);
private:
    std::vector<std::filesystem::path> cpps;   //все .cpp файлы, которые были найдены в source-path
    std::vector<std::filesystem::path> headers;//все   .h файлы, которые были найдены в source-path
    std::vector<std::filesystem::path> libs;   //пути к библиотекам, которые передаются в программу в качестве аргументов

    std::stringstream tree;
    std::map<std::filesystem::path, int> entryes;
};


#endif //INCLUDEANALYSER_INCLUDEANALYSER_H
