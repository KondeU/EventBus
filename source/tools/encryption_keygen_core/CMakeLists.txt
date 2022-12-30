cmake_minimum_required(VERSION 3.14)
project(encryption_keygen_core)

file(GLOB_RECURSE SRC *.cpp)
add_library(${PROJECT_NAME} STATIC ${SRC} ${INC})

install_artifact(${PROJECT_NAME})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../../.. FILES ${SRC} ${INC})
