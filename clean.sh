#!/bin/bash

CURRENT_PATH=$(cd "$(dirname "$0")"; pwd)

rm -rf ${CURRENT_PATH}/build
rm -rf ${CURRENT_PATH}/output

bash ${CURRENT_PATH}/sample/clean.sh
bash ${CURRENT_PATH}/3rd/remove_cache.sh
