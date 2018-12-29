#!/usr/bin/env bash
if [[ "$1" = "dev" ]]; then
    git clone git@github.com:boostorg/boost.git
else
    git clone https://github.com/boostorg/boost.git
fi
cd boost
git checkout boost-1.65.1
git submodule update --init tools/boostdep tools/build
git submodule update --init libs/filesystem libs/test
python tools/boostdep/depinst/depinst.py filesystem
python tools/boostdep/depinst/depinst.py test
bash ./bootstrap.sh gcc
./b2 -j4 --with-filesystem --with-test --build-type=minimal variant=release link=shared --toolset=gcc address-model=64 stage --layout=system