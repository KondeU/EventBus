file(GLOB_RECURSE EVENT_BUS_HEADERS include/*)

macro(target_use_event_bus target_artifact)
  target_include_directories(
    ${target_artifact} PUBLIC
      external
      include
      include/common
      include/communicate
      include/serialize
      include/utils
  )
  target_link_directories(
    ${target_artifact} PUBLIC
      library
  )
  target_link_libraries(
    ${target_artifact} PUBLIC
      event_bus_open_source
  )
endmacro()
