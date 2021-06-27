cmake_minimum_required(VERSION 3.10)
project(Common)

file(GLOB_RECURSE Communicate communicate/*)
file(GLOB_RECURSE Serialize serialize/*)
file(GLOB_RECURSE Process process/*)

add_library(common STATIC ${Communicate} ${Serialize} ${Process})
target_link_libraries(common libzmq)

if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.16.0")
#  target_precompile_headers(common PRIVATE serialize/?)
endif()
