cmake_minimum_required(VERSION 3.14)
project(sample)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
  message(STATUS "ARCHITECTURE is x86.")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
  message(STATUS "ARCHITECTURE is x64.")
endif()

include(../output/EventBus.cmake)

add_subdirectory(01_event_bus_simple_demo)
add_subdirectory(02_easy_rpc_simple_demo)

install(DIRECTORY ../output/runtime/ DESTINATION .)
