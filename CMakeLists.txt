cmake_minimum_required(VERSION 3.10)

project(midiwav)

set(CMAKE_PREFIX_PATH "C:/vcpkg/installed/x64-windows")

find_package(raylib REQUIRED)
add_executable(midiwav src/mainA.cpp)

target_link_libraries(midiwav PRIVATE raylib)
