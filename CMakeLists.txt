cmake_minimum_required(VERSION 3.18)
project(longrangered)
set(CMAKE_CXX_STANDARD 17)

add_library(longrangered SHARED main.cpp And64InlineHook.cpp)

target_include_directories(longrangered PRIVATE .)

target_link_libraries(longrangered
    log
    dl
    m
)
