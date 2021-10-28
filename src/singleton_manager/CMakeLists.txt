cmake_minimum_required(VERSION 3.10)
project(singleton_manager)

file(GLOB_RECURSE SRC *.cpp)
add_library(singleton_manager SHARED ${SRC} ${INC})
