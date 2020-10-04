# cmake_minimum_required(VERSION <specify CMake version here>)
project(nonogram)

set(CMAKE_CXX_STANDARD 17)

add_executable(nonogram main.cpp
        logging/Logger.cpp
        logging/Logger.h
        nonorgram/Nonogram.cpp
        nonorgram/Nonogram.h)