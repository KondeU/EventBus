cmake_minimum_required(VERSION 3.10)
project(Topicer)

add_definitions(-DTIBUS_EXPORT)

file(GLOB_RECURSE SRC *.cpp *.h)

add_library(TiBusInproc SHARED ${SRC})
target_link_libraries(TiBusInproc common)

if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.16.0")
#  target_precompile_headers(TiBusInproc PRIVATE ?.h)
endif()
