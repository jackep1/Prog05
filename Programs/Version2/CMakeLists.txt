cmake_minimum_required(VERSION 3.29)
project(Version2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Version2
    main.cpp
        Source/line_sort.cpp
        Source/file_ops.cpp)
