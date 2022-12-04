cmake_minimum_required(VERSION 3.14)
project(EventBus)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)
file(GLOB_RECURSE INC include/*)

macro(install_artifact target_artifact)
install(TARGETS ${target_artifact}
  ARCHIVE DESTINATION library # .lib/.a
  LIBRARY DESTINATION runtime # .dll/.so
  RUNTIME DESTINATION runtime # .exe/.out
  PUBLIC_HEADER DESTINATION include/external)
endmacro()

add_subdirectory(3rd)
#add_subdirectory(source)

install(DIRECTORY include/ DESTINATION include)
install(FILES User.cmake DESTINATION . RENAME EventBus.cmake)
install(FILES LICENSE.txt DESTINATION .)
install(FILES README.md DESTINATION .)
