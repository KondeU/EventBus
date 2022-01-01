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
)

link_directories(
  ../build/dependency/zeromq/zeromq/lib
)

if(MSVC)
  link_libraries(libzmq)
else()
  link_libraries(zmq)
endif()

file(GLOB_RECURSE INC ../include/*)

add_subdirectory(local_broker)
add_subdirectory(hosts_broker)
add_subdirectory(config_center)
add_subdirectory(singleton_manager)
add_subdirectory(encryption_keygen)
