cmake_minimum_required(VERSION 3.10)

project(MyProject VERSION 1.0 LANGUAGES CXX)


find_package(Threads REQUIRED)
add_executable(task3 task3.cpp)

target_include_directories(task3 PRIVATE cpr/build/include/)

target_link_directories(task3 PRIVATE cpr/build/lib/)

target_link_libraries(task3 PRIVATE cpr Threads::Threads)

