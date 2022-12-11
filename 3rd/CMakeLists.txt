cmake_minimum_required(VERSION 3.14)
project(event_bus_open_source)

set(OPEN_SOURCE_WRAPPERS open_source.cpp)
add_library(${PROJECT_NAME} STATIC ${OPEN_SOURCE_WRAPPERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/.. FILES ${OPEN_SOURCE_WRAPPERS})

install_artifact(${PROJECT_NAME})

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
  set(THIRD_PARTY_ARCHITECTURE Win32)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(THIRD_PARTY_ARCHITECTURE x64)
endif()
set(THIRD_PARTY_EXTERNAL ${CMAKE_CURRENT_SOURCE_DIR}/external)
set(THIRD_PARTY_INTERNAL ${CMAKE_CURRENT_SOURCE_DIR}/internal)

macro(install_external_open_source_headers headers_directory)
  install(DIRECTORY ${headers_directory} DESTINATION external)
endmacro()

#==============================================================================

add_custom_target(unzip_cereal ALL)
add_custom_command(TARGET unzip_cereal PRE_BUILD
  COMMENT "unpacking cereal."
  DEPENDS cereal-1.3.2.zip
  WORKING_DIRECTORY ${THIRD_PARTY_EXTERNAL}
    COMMAND ${CMAKE_COMMAND} -E remove_directory cereal
    COMMAND ${CMAKE_COMMAND} -E tar xzf cereal-1.3.2.zip
    COMMAND ${CMAKE_COMMAND} -E rename cereal-1.3.2 cereal
  VERBATIM)

target_include_directories(${PROJECT_NAME} PUBLIC ${THIRD_PARTY_EXTERNAL}/cereal/include)
install_external_open_source_headers(${THIRD_PARTY_EXTERNAL}/cereal/include/)

#==============================================================================

add_custom_target(unzip_msgpack ALL)
add_custom_command(TARGET unzip_msgpack PRE_BUILD
  COMMENT "unpacking msgpack."
  DEPENDS msgpack-c-cpp-4.1.3.zip
  WORKING_DIRECTORY ${THIRD_PARTY_EXTERNAL}
    COMMAND ${CMAKE_COMMAND} -E remove_directory msgpack
    COMMAND ${CMAKE_COMMAND} -E tar xzf msgpack-c-cpp-4.1.3.zip
    COMMAND ${CMAKE_COMMAND} -E rename msgpack-c-cpp-4.1.3 msgpack
  VERBATIM)

target_include_directories(${PROJECT_NAME} PUBLIC ${THIRD_PARTY_EXTERNAL}/msgpack/include)
install_external_open_source_headers(${THIRD_PARTY_EXTERNAL}/msgpack/include/)

#==============================================================================

add_custom_target(unzip_zeromq ALL)
add_custom_command(TARGET unzip_zeromq PRE_BUILD
  COMMENT "unpacking zeromq, compile and install it."
  DEPENDS zeromq-4.3.4.zip
  WORKING_DIRECTORY ${THIRD_PARTY_EXTERNAL}
    COMMAND ${CMAKE_COMMAND} -E remove_directory zeromq
    COMMAND ${CMAKE_COMMAND} -E tar xzf zeromq-4.3.4.zip
    COMMAND ${CMAKE_COMMAND} -E rename zeromq-4.3.4 zeromq
    COMMAND patch -p0 < zeromq-4.3.4@0001.patch --binary
    COMMAND cd zeromq
    COMMAND ${CMAKE_COMMAND} -G ${CMAKE_GENERATOR} -S . -B build -A ${THIRD_PARTY_ARCHITECTURE}
      -DBUILD_STATIC=OFF -DBUILD_SHARED=ON -DBUILD_TESTS=OFF -DZMQ_BUILD_TESTS=OFF
      -DWITH_LIBSODIUM=OFF -DENABLE_DRAFTS=OFF -Wno-dev
    COMMAND ${CMAKE_COMMAND} --build build --config Release --parallel
    COMMAND ${CMAKE_COMMAND} --install build --prefix output --config Release
    COMMAND cd ..
  VERBATIM)

target_include_directories(${PROJECT_NAME} PUBLIC ${THIRD_PARTY_EXTERNAL}/zeromq/output/include)
install_external_open_source_headers(${THIRD_PARTY_EXTERNAL}/zeromq/output/include/)

target_link_directories(${PROJECT_NAME} PUBLIC ${THIRD_PARTY_EXTERNAL}/ZeroMQ/output/lib)
if(MSVC)
  target_link_libraries(${PROJECT_NAME} PUBLIC libzmq)
else()
  target_link_libraries(${PROJECT_NAME} PUBLIC zmq)
endif()

if(WIN32)
  install(FILES ${THIRD_PARTY_EXTERNAL}/ZeroMQ/output/bin/libzmq.dll DESTINATION runtime)
else()
  install(FILES ${THIRD_PARTY_EXTERNAL}/ZeroMQ/output/bin/libzmq.so DESTINATION runtime)
endif()

#==============================================================================

add_custom_target(unzip_cppzmq ALL)
add_custom_command(TARGET unzip_cppzmq PRE_BUILD
  COMMENT "unpacking cppzmq."
  DEPENDS cppzmq-4.9.0.zip
  WORKING_DIRECTORY ${THIRD_PARTY_EXTERNAL}
    COMMAND ${CMAKE_COMMAND} -E remove_directory cppzmq
    COMMAND ${CMAKE_COMMAND} -E tar xzf cppzmq-4.9.0.zip
    COMMAND ${CMAKE_COMMAND} -E rename cppzmq-4.9.0 cppzmq
    COMMAND cd cppzmq
    COMMAND ${CMAKE_COMMAND} -G ${CMAKE_GENERATOR} -S . -B build -A ${THIRD_PARTY_ARCHITECTURE}
      -DCPPZMQ_BUILD_TESTS=OFF -DZeroMQ_DIR=${THIRD_PARTY_EXTERNAL}/ZeroMQ/output/CMake -Wno-dev
    COMMAND ${CMAKE_COMMAND} --build build --config Release --parallel
    COMMAND ${CMAKE_COMMAND} --install build --prefix output --config Release
    COMMAND cd ..
  VERBATIM)

target_include_directories(${PROJECT_NAME} PUBLIC ${THIRD_PARTY_EXTERNAL}/cppzmq/output/include)
install_external_open_source_headers(${THIRD_PARTY_EXTERNAL}/cppzmq/output/include/)

#==============================================================================

if(WIN32)
  add_custom_target(unzip_patch ALL)
  add_custom_command(TARGET unzip_patch PRE_BUILD
    COMMENT "unpacking patch, extract patch tool in Windows."
    DEPENDS patch-2.5.9_from_visual_studio_2017_strawberry_c_bin.zip
    WORKING_DIRECTORY ${THIRD_PARTY_EXTERNAL}
      COMMAND ${CMAKE_COMMAND} -E remove patch.exe
      COMMAND ${CMAKE_COMMAND} -E remove_directory patch_exe
      COMMAND ${CMAKE_COMMAND} -E make_directory patch_exe
      COMMAND cd patch_exe
      COMMAND ${CMAKE_COMMAND} -E tar xzf
        ../patch-2.5.9_from_visual_studio_2017_strawberry_c_bin.zip
      COMMAND ${CMAKE_COMMAND} -E copy patch.exe ..
      COMMAND cd ..
    VERBATIM)
  add_dependencies(unzip_zeromq unzip_patch)
endif()

#==============================================================================

add_custom_target(unzip_global_singleton ALL)
add_custom_command(TARGET unzip_global_singleton PRE_BUILD
  COMMENT "unpacking GlobalSingleton, compile and install it."
  DEPENDS GlobalSingleton-1.0.zip
  WORKING_DIRECTORY ${THIRD_PARTY_INTERNAL}
    COMMAND ${CMAKE_COMMAND} -E remove_directory gsgt
    COMMAND ${CMAKE_COMMAND} -E tar xzf GlobalSingleton-1.0.zip
    COMMAND ${CMAKE_COMMAND} -E rename GlobalSingleton-1.0 gsgt
    COMMAND cd gsgt
    COMMAND ${CMAKE_COMMAND} -G ${CMAKE_GENERATOR} -S . -B build -A ${THIRD_PARTY_ARCHITECTURE}
    COMMAND ${CMAKE_COMMAND} --build build --config Release --parallel
    COMMAND ${CMAKE_COMMAND} --install build --prefix output --config Release
    COMMAND cd ..
  VERBATIM)

target_include_directories(${PROJECT_NAME} PUBLIC ${THIRD_PARTY_INTERNAL}/gsgt/output/include)
install(DIRECTORY ${THIRD_PARTY_INTERNAL}/gsgt/output/include/ DESTINATION include/common)

if(WIN32)
  install(FILES ${THIRD_PARTY_INTERNAL}/gsgt/output/runtime/sgtmgr.dll DESTINATION runtime)
else()
  install(FILES ${THIRD_PARTY_INTERNAL}/gsgt/output/runtime/libsgtmgr.so DESTINATION runtime)
endif()

#==============================================================================

add_dependencies(${PROJECT_NAME} unzip_cereal)
add_dependencies(${PROJECT_NAME} unzip_msgpack)
add_dependencies(${PROJECT_NAME} unzip_cppzmq)
add_dependencies(unzip_cppzmq unzip_zeromq)
add_dependencies(${PROJECT_NAME} unzip_global_singleton)
