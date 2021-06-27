cmake_minimum_required(VERSION 3.10)
project(Launcher)

option(TIBUS_SERVER_USE_CONSOLE "Server showes console" ON)
if(NOT TIBUS_SERVER_USE_CONSOLE)
  if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /subsystem:windows /entry:mainCRTStartup")
  else()
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--subsystem,windows")
  endif()
endif()

file(GLOB_RECURSE SRC *.cpp *.h)

add_executable(Launcher ${SRC})
target_link_libraries(Launcher common)

if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.16.0")
#  target_precompile_headers(Launcher PRIVATE ?.h)
endif()
