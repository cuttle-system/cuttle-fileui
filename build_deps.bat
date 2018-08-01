cd deps\boost
git submodule update --init
.\bootstrap gcc
.\b2 --with-filesystem --build-type=minimal variant=release --toolset=gcc address-model=64 stage --layout=system

cd ..\..