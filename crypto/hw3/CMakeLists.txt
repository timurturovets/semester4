cmake_minimum_required(VERSION 4.1)
project(hw3)

set(CMAKE_CXX_STANDARD 20)

add_executable(hw3 main.cpp
        menu.cpp
        task1/run.h
        task1/task1.cpp
        task1/task1.h)
