#!/bin/bash

set -e
CURRENT_PATH=$(cd "$(dirname "$0")"; pwd)

cmake -S . -B build -A x64
cmake --build build --config Release --parallel
cmake --install build --prefix output --config Release
