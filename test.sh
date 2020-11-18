#!/usr/bin/env sh
mkdir -p build
cd build
cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
make -j
./tests
