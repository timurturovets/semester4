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
        task4/rc4/rc4.h
        task4/rc4/rc4.cpp
        task4/run.h
        task4/task4.cpp
        task4/auxiliary/shared_memory.h
        task4/ipc/server.cpp
        task4/ipc/client.cpp
        task4/ipc/ipc.h
)
