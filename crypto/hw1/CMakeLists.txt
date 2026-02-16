cmake_minimum_required(VERSION 4.1)
project(hw1)

set(CMAKE_CXX_STANDARD 20)

add_executable(hw1 main.cpp
        task1/task1.cpp
        task1/task1.h
        task1/run.h
        menu.cpp
)
