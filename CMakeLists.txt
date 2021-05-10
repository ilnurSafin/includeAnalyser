cmake_minimum_required(VERSION 3.14)
project(includeAnalyser)

set(CMAKE_CXX_STANDARD 17)

add_executable(analyser main.cpp InputParser.h IncludeAnalyser.cpp IncludeAnalyser.h)