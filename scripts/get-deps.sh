#!/bin/bash
cd $(dirname "$0")/../..

git clone git@github.com:boostorg/boost.git
cd boost
git checkout boost-1.65.1
git submodule update --init
./bootstrap.sh gcc
./b2 --with-filesystem --build-type=minimal variant=release --toolset=gcc address-model=64 stage --layout=system 

cd $(dirname "$0")/../..

git clone git@github.com:cuttle-system/cuttle-parser.git
cuttle-parser/scripts/get-deps.sh

cd $(dirname "$0")/../..

git clone git@github.com:cuttle-system/cuttle-translator.git
cuttle-translator/scripts/get-deps.sh

cd $(dirname "$0")/../..

git clone git@github.com:cuttle-system/cuttle-generator.git
cuttle-generator/scripts/get-deps.sh

cd $(dirname "$0")/../..

git clone git@github.com:cuttle-system/cuttle-vm.git
cuttle-vm/scripts/get-deps.sh

cd $(dirname "$0")/../..

git clone git@github.com:cuttle-system/cuttle-lang.git
cuttle-lang/scripts/get-deps.sh