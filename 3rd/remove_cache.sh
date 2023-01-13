#!/bin/bash

CURRENT_PATH=$(cd "$(dirname "$0")"; pwd)

rm -f ${CURRENT_PATH}/external/patch.exe
rm -rf ${CURRENT_PATH}/external/patch_exe
rm -rf ${CURRENT_PATH}/external/cereal
rm -rf ${CURRENT_PATH}/external/msgpack
rm -rf ${CURRENT_PATH}/external/zeromq
rm -rf ${CURRENT_PATH}/external/cppzmq
#rm -rf ${CURRENT_PATH}/internal/gsgt
