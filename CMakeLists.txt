# cmake_minimum_required(VERSION <specify CMake version here>)
project(nonogram)

set(CMAKE_CXX_STANDARD 17)

add_executable(nonogram main.cpp
        line/Line.cpp
        line/Line.h
        logging/Logger.cpp
        logging/Logger.h
        nonogram/Nonogram.cpp
        nonogram/Nonogram.h)
