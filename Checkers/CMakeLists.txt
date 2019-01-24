cmake_minimum_required(VERSION 3.13)
project(Checkers)

set(CMAKE_CXX_STANDARD 14)

add_executable(Checkers project/Main.cpp project/Coord.h project/State.cpp project/State.h project/Turn.cpp project/Turn.h)