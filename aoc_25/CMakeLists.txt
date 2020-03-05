cmake_minimum_required(VERSION 2.9)

set(PROJECT_NAME aoc_25)
project(${PROJECT_NAME})

set(CMAKE_CXX_FLAGS -g)

add_executable(${PROJECT_NAME} src/main.cpp src/computer.cpp)

