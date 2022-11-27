cmake_minimum_required(VERSION 3.10)
project(local_broker)

file(GLOB_RECURSE SRC *.cpp)
add_executable(local_broker ${SRC} ${INC})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../.. FILES ${SRC} ${INC})
