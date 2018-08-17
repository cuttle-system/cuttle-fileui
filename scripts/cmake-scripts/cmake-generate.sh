#!/bin/bash
cd $(dirname "$0")/../..

mkdir -p build
cd build
rm -rf *
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ..
