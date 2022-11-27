cmake_minimum_required(VERSION 3.10)
project(hosts_broker)

file(GLOB_RECURSE SRC *.cpp)
add_executable(hosts_broker ${SRC} ${INC})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../.. FILES ${SRC} ${INC})
