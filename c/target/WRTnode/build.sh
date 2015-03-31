#! /bin/bash
rm -rf build
mkdir build
cp toolchain.cmake build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=./toolchain.cmake ../../../
