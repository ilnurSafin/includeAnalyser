#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <map>

#include "InputParser.h"
#include "IncludeAnalyser.h"


int main(int argc, char **argv)
try {
    InputParser parser(argc, argv);
    IncludeAnalyser analyser(parser.getSourcePath(), parser.getLibPaths());

    //запись результатов также осуществляется в файл
    std::ofstream logFile("log.txt");

    //вывод дерева зависимостей
    std::cout << analyser.getFileDependeciesTree() << std::endl;
    logFile << analyser.getFileDependeciesTree() << std::endl;

    //внутри класса я использовал map, а он по-умолчанию сортирует пары только по ключам. Но, т.к. в задании указана сортировка сначала по убыванию частоты, а затем по алфавиту, то пришлось прибегнуть к костылю :)
    auto entryes = analyser.getFileEntryes();
    std::vector<std::pair<std::filesystem::path,int>> entryesCopy(entryes.begin(),entryes.end());
    std::sort(entryesCopy.begin(), entryesCopy.end(), [](std::pair<std::filesystem::path,int> const & a, std::pair<std::filesystem::path,int> const & b){
        return a.second != b.second?  a.second > b.second : std::tolower(a.first.string()[0]) < std::tolower(b.first.string()[0]);
    });

    //вывод списка файлов и частоты их влючений
    for (auto &entry : entryesCopy) {
        std::cout << entry.first << " " << entry.second << std::endl;
        logFile <<  entry.first << " " << entry.second << std::endl;
    }
    logFile.close();
} catch (const std::exception& e) {
    std::cout << e.what() << std::endl;
}
