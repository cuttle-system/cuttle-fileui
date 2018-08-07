function commit() {
    git checkout master
    git submodule update --remote --recursive
    git add --all
    git commit
    git pull
    git push origin master
}

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

commit
