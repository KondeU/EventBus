cmake_minimum_required(VERSION 3.10)
project(encryption_keygen)

file(GLOB_RECURSE SRC *.cpp)
add_executable(encryption_keygen ${SRC} ${INC})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../.. FILES ${SRC} ${INC})
