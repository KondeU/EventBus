cmake_minimum_required(VERSION 3.10)
project(config_center)

file(GLOB_RECURSE SRC *.cpp)
add_executable(config_center ${SRC})
