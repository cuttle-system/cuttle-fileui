git clone --recursive git@github.com:boostorg/boost.git deps/boost
cd deps/boost
./bootstrap gcc
./b2 --with-filesystem --build-type=minimal variant=release --toolset=gcc address-model=64 stage --layout=system
cd ../..
