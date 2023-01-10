cmake_minimum_required(VERSION 3.5)

project(script-reader LANGUAGES CXX)
set(PROJECT_VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/scriptreader)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/examples)

target_include_directories(example-npc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/scriptreader/)

