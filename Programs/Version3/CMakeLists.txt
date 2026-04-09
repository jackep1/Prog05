cmake_minimum_required(VERSION 3.29)
project(Version3)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Version3 main.cpp
        Source/file_ops.cpp
        Source/line_sort.cpp)
add_executable(Dv3 distributor.cpp
        Source/file_ops.cpp
        Source/line_sort.cpp)
add_executable(Pv3 processor.cpp
        Source/file_ops.cpp
        Source/line_sort.cpp)
