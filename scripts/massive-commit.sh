#!/bin/bash
commit() {
    git checkout master
    git pull
    git submodule update --remote --recursive
    git add --all
    git commit
    git pull
    git push origin master
}

cd $(dirname "$0")/..

cd tests/cuttle-test
commit
cd ../..

cd deps/cuttle-parser
commit
cd ../..

cd deps/cuttle-translator
commit
cd ../..

cd deps/cuttle-generator
commit
cd ../..

cd deps/cuttle-vm
commit
cd ../..

cd deps/cuttle-lang
commit
cd ../..

commit
