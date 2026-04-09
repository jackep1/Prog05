cmake_minimum_required(VERSION 3.29)
project(Version4)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Version4 main.cpp
        Source/file_ops.cpp
        Headers/file_ops.h
        Source/line_sort.cpp
        Headers/line_sort.h)
add_executable(Dv4 distributor.cpp
        Source/file_ops.cpp
        Headers/file_ops.h
        Source/line_sort.cpp
        Headers/line_sort.h)
add_executable(Pv4 processor.cpp
        Source/file_ops.cpp
        Headers/file_ops.h
        Source/line_sort.cpp
        Headers/line_sort.h)
