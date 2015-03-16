cmake_minimum_required(VERSION 2.8.4)
project(ProgtestOSY1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -pedantic -pthread")

set(SOURCE_FILES
    seqmain.cpp
    solution.cpp
    speedmain.cpp
    thrmain.cpp)

include_directories(.)

add_executable(ProgtestOSY1 seqmain.cpp)