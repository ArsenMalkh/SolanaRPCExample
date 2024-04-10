cmake_minimum_required(VERSION 3.10)

project(MyProject VERSION 1.0 LANGUAGES CXX)


find_package(Threads REQUIRED)

add_executable(task2 task2.cpp)

target_include_directories(task2 PRIVATE cpr/build/include/)

target_link_directories(task2 PRIVATE cpr/build/lib/)

target_link_libraries(task2 PRIVATE cpr Threads::Threads)

