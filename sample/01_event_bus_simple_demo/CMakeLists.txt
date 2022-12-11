cmake_minimum_required(VERSION 3.14)
project(01_event_bus_simple_demo)

file(GLOB_RECURSE worker1_src bus_def/* worker1/*)
file(GLOB_RECURSE worker2_src bus_def/* worker2/*)

add_executable(01_event_bus_simple_demo_worker1 ${worker1_src} ${EVENT_BUS_HEADERS})
add_executable(01_event_bus_simple_demo_worker2 ${worker2_src} ${EVENT_BUS_HEADERS})

target_use_event_bus(01_event_bus_simple_demo_worker1)
target_use_event_bus(01_event_bus_simple_demo_worker2)

install(TARGETS
    01_event_bus_simple_demo_worker1
    01_event_bus_simple_demo_worker2
  RUNTIME DESTINATION .
)
