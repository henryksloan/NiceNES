cmake_minimum_required(VERSION 3.0)
project(NiceNES)

add_subdirectory(CPU6502)

# TODO: Add Release and Debug settings, avoiding -g on releases
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -std=c++17")
include_directories(include)
include_directories(CPU6502/include)
link_directories(CPU6502/src)
file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(NiceNES ${SOURCES})
target_link_libraries(NiceNES CPU6502)
