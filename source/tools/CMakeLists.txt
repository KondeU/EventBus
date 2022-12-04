cmake_minimum_required(VERSION 3.14)
project(tools)



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





add_subdirectory(local_broker_core)
add_subdirectory(local_broker_simple)
add_subdirectory(hosts_broker_core)
add_subdirectory(hosts_broker_simple)
add_subdirectory(encryption_keygen_core)
add_subdirectory(encryption_keygen_simple)
