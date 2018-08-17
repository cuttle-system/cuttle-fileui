cd $(dirname "$0")/..
git clone git@github.com:boostorg/boost.git deps/boost
cd deps/boost
git checkout boost-1.65.1
git submodule update --init
./bootstrap.sh gcc
./b2 --with-filesystem --build-type=minimal variant=release --toolset=gcc address-model=64 stage --layout=system
cd ../..
