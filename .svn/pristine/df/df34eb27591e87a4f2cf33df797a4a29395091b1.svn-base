#!/bin/bash
rm -f 0DEBUG
touch 0RELEASE
sh buildcmake.sh
cmake -DCMAKE_CXX_COMPILER=/usr/bin/g++ -DCMAKE_C_COMPILER=/usr/bin/gcc -DCMAKE_BUILD_TYPE=release -DEXECUTABLE_OUTPUT_PATH=./app_release/bin_release .
make
