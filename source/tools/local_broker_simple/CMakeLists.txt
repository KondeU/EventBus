cmake_minimum_required(VERSION 3.14)
project(local_broker_simple)

file(GLOB_RECURSE SRC *.cpp)
add_executable(${PROJECT_NAME} ${SRC} ${INC})
target_link_libraries(${PROJECT_NAME} local_broker_core)

install_artifact(${PROJECT_NAME})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../../.. FILES ${SRC} ${INC})
