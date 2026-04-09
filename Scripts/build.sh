#!/bin/bash

# Optionally take an argument for a version to build
if [ -n "$1" ]
then
    if [ "$1" == "v1" ]
    then
        echo "Starting Build"
        g++ -Wall -std=c++20 ./Programs/Version1/Source/*.cpp ./Programs/Version1/main.cpp -o v1
        echo "Build Complete"
    elif [ "$1" == "v2" ]
    then
        echo "Starting Build"
        g++ -Wall -std=c++20 ./Programs/Version2/Source/*.cpp ./Programs/Version2/main.cpp -o v2
        echo "Build Complete"
    elif [ "$1" == "v3" ]
    then
        echo "Starting Build"
        g++ -Wall -std=c++20 ./Programs/Version3/Source/*.cpp ./Programs/Version3/main.cpp -o v3
        g++ -Wall -std=c++20 ./Programs/Version3/Source/*.cpp ./Programs/Version3/distributor.cpp -o Dv3
        g++ -Wall -std=c++20 ./Programs/Version3/Source/*.cpp ./Programs/Version3/processor.cpp -o Pv3
        echo "Build Complete"
    elif [ "$1" == "v4" ]
    then
        echo "Starting Build"
        g++ -Wall -std=c++20 ./Programs/Version4/Source/*.cpp ./Programs/Version4/main.cpp -o v4
        g++ -Wall -std=c++20 ./Programs/Version4/Source/*.cpp ./Programs/Version4/distributor.cpp -o Dv4
        g++ -Wall -std=c++20 ./Programs/Version4/Source/*.cpp ./Programs/Version4/processor.cpp -o Pv4
        echo "Build Complete"
    elif [ "$1" == "v5" ]
    then
        echo "Starting Build"
        g++ -Wall -std=c++20 ./Programs/Version5/Source/*.cpp ./Programs/Version5/main.cpp -o v5
        g++ -Wall -std=c++20 ./Programs/Version5/Source/*.cpp ./Programs/Version5/distributor.cpp -o Dv5
        g++ -Wall -std=c++20 ./Programs/Version5/Source/*.cpp ./Programs/Version5/processor.cpp -o Pv5
        echo "Build Complete"
    else
        echo "Invalid version specified"
    fi
    exit
fi

echo "Building Version 1"
g++ -Wall -std=c++20 ./Programs/Version1/Source/*.cpp ./Programs/Version1/main.cpp -o v1

echo "Building Version 2"
g++ -Wall -std=c++20 ./Programs/Version2/Source/*.cpp ./Programs/Version2/main.cpp -o v2

echo "Building Version 3"
g++ -Wall -std=c++20 ./Programs/Version3/Source/*.cpp ./Programs/Version3/main.cpp -o v3
g++ -Wall -std=c++20 ./Programs/Version3/Source/*.cpp ./Programs/Version3/distributor.cpp -o Dv3
g++ -Wall -std=c++20 ./Programs/Version3/Source/*.cpp ./Programs/Version3/processor.cpp -o Pv3

echo "Building Version 4"
g++ -Wall -std=c++20 ./Programs/Version4/Source/*.cpp ./Programs/Version4/main.cpp -o v4
g++ -Wall -std=c++20 ./Programs/Version4/Source/*.cpp ./Programs/Version4/distributor.cpp -o Dv4
g++ -Wall -std=c++20 ./Programs/Version4/Source/*.cpp ./Programs/Version4/processor.cpp -o Pv4

echo "Building Version 5"
g++ -Wall -std=c++20 ./Programs/Version5/Source/*.cpp ./Programs/Version5/main.cpp -o v5
g++ -Wall -std=c++20 ./Programs/Version5/Source/*.cpp ./Programs/Version5/distributor.cpp -o Dv5
g++ -Wall -std=c++20 ./Programs/Version5/Source/*.cpp ./Programs/Version5/processor.cpp -o Pv5