cmake_minimum_required(VERSION 3.14)
project(source)

link_libraries(event_bus_open_source)

add_subdirectory(tools)
add_subdirectory(test)
