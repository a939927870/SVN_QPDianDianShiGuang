#!/bin/bash

rm -f 0RELEASE
touch 0DEBUG
sh buildcmake.sh
cmake -DCMAKE_CXX_COMPILER=/usr/bin/g++ -DCMAKE_C_COMPILER=/usr/bin/gcc -DCMAKE_BUILD_TYPE=debug -DEXECUTABLE_OUTPUT_PATH=./app_release/bin_debug .
make
