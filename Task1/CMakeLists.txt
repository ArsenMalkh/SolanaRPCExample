# Set the minimum version of CMake required to build this project
cmake_minimum_required(VERSION 3.10)


project(MyProject VERSION 1.0 LANGUAGES CXX)


add_executable(task1 task1.cpp)

target_include_directories(task1 PRIVATE cpr/build/include/)
target_link_directories(task1 PRIVATE cpr/build/lib/)

target_link_libraries(task1 PRIVATE cpr)

