set(EVENT_BUS_DIR ${CMAKE_CURRENT_LIST_DIR})

file(GLOB_RECURSE EVENT_BUS_HEADERS ${EVENT_BUS_DIR}/include/*)

macro(target_use_event_bus target_artifact)
  target_include_directories(
    ${target_artifact} PUBLIC
      ${EVENT_BUS_DIR}/external
      ${EVENT_BUS_DIR}/include
      ${EVENT_BUS_DIR}/include/common
      ${EVENT_BUS_DIR}/include/communicate
      ${EVENT_BUS_DIR}/include/serialize
      ${EVENT_BUS_DIR}/include/utils
  )
  target_link_directories(
    ${target_artifact} PUBLIC
      ${EVENT_BUS_DIR}/library
  )
  target_link_libraries(
    ${target_artifact} PUBLIC
      event_bus_open_source
  )
  if(WIN32)
    target_link_libraries(${target_artifact} PUBLIC libzmq)
  else()
    target_link_libraries(${target_artifact} PUBLIC zmq)
  endif()
endmacro()
