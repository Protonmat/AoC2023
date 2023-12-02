cmake_minimum_required(VERSION 3.27.7)
project(01)


set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_VERBOSE_MAKEFILE off)

file(COPY input.txt DESTINATION ./)
add_executable(${PROJECT_NAME} main.cpp)