cmake_minimum_required(VERSION 3.19)
project(Text_bird C)
add_definitions(-w)
set(CMAKE_C_STANDARD 99)
link_libraries(ncurses libpthread.so)
add_executable(Text_bird game.c playaudio.c playaudio.h)