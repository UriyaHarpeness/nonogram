cmake_minimum_required(VERSION 3.10.2)
project(nonogram)

set(CMAKE_CXX_STANDARD 17)

add_executable(nonogram main.cpp
        line/Line.cpp
        line/Line.h
        logging/debug.h
        logging/Logger.cpp
        logging/Logger.h
        nonogram/Nonogram.cpp
        nonogram/Nonogram.h)
