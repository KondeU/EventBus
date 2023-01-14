#!/bin/bash

set -e
CURRENT_PATH=$(cd "$(dirname "$0")"; pwd)
cd ${CURRENT_PATH}

cmake -S . -B build
cmake --build build --config Release --parallel
cmake --install build --prefix output --config Release
