cmake_minimum_required(VERSION 3.10)
project(Common)

file(GLOB_RECURSE SRC *.cpp *.h)
file(GLOB_RECURSE INC ../../include/*.hpp)

source_group(Exports FILES ${INC})

add_library(common STATIC ${SRC} ${INC})
target_link_libraries(common libzmq)
