cmake_minimum_required(VERSION 4.1)
project(hw1)

set(CMAKE_CXX_STANDARD 20)

add_executable(hw1 main.cpp
        task1/task1.cpp
        task1/task1.h
        task1/run.h
        menu.cpp
        task2/run.h
        task2/task2.cpp
        task2/task2.h
        task3/run.h
        task3/task3.h
        task3/task3.cpp
)
