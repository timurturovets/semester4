cmake_minimum_required(VERSION 4.1)
project(hw6)

set(CMAKE_CXX_STANDARD 20)

add_executable(hw6 main.cpp
        menu.cpp
        task1/run.h
        task1/task1.h
        task1/task1.cpp
        task2/task2.h
        task2/task2.cpp
        task2/run.h)
