cmake_minimum_required(VERSION 3.10)
project(TiBus)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(../include
  ../build/dependency/cereal/include
  ../build/dependency/msgpack/include
  ../build/dependency/zeromq/zeromq/include
  ../build/dependency/zeromq/cppzmq/include
  common
)

link_directories(
  ../build/dependency/zeromq/zeromq/lib
)

add_subdirectory(common)
add_subdirectory(topicer)
add_subdirectory(launcher)
add_subdirectory(router)
