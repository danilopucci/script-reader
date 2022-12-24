cmake_minimum_required(VERSION 3.5)

project(script-reader LANGUAGES CXX)
set(PROJECT_VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(script-reader src/main.cpp src/readscriptfile.h src/readscriptfile.cpp)
