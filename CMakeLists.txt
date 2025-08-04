cmake_minimum_required(VERSION 3.10)

project(midiwav)

add_executable(midiwav src/mainA.cpp)
add_executable(decoder src/note_decoder.c)
