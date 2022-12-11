cmake_minimum_required(VERSION 3.14)
project(02_easy_rpc_simple_demo)

file(GLOB_RECURSE server_src server/*)
file(GLOB_RECURSE client_src client/*)

add_executable(02_easy_rpc_simple_demo_server ${server_src} ${EVENT_BUS_HEADERS})
add_executable(02_easy_rpc_simple_demo_client ${client_src} ${EVENT_BUS_HEADERS})

target_use_event_bus(02_easy_rpc_simple_demo_server)
target_use_event_bus(02_easy_rpc_simple_demo_client)

install(TARGETS
    02_easy_rpc_simple_demo_server
    02_easy_rpc_simple_demo_client
  RUNTIME DESTINATION .
)
