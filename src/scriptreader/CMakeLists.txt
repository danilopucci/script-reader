add_library(scriptreader
    streambuffer.h
    scripttoken.h scripttoken.cpp
    scriptfile.h scriptfile.cpp
    readscriptfile.h readscriptfile.cpp
    )

target_include_directories(scriptreader PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
