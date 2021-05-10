# includeAnalyser
C++17 <br>
Compiler Migw64 10.2.0<br>
IDE: CLion 2019.2<br>
<br>
Решение задания для собеседования в Тензор.<br>
Программа выводит дерево зависимостей файлов по директиве *#include* и их список с частотой включения.<br>
<br>
Паттерн запуска: analyser "sources path" [options]<br>
Опции:
* -I – путь для поиска исходных файлов 
<br>
Пример1(анализа самого проекта):   analyser "c:\Projects\CLionProjects\includeAnalyser\" -I "C:\Program Files\MSYS2\mingw64\include\c++\10.2.0"<br>
Вывод1: см. log1.txt<br>
Пример2(анализа самого проекта без указания пути к библиотеке/ам):   analyser "c:\Projects\CLionProjects\includeAnalyser\"<br>
Вывод2: см. log2.txt<br>
<br>
PS: тестировал только под win10
