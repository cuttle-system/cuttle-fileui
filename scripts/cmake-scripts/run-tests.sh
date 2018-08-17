#!/bin/bash
cd $(dirname "$0")/../..

./scripts/cmake-scripts/cmake-build.sh
cd build
./$1
cd ..
