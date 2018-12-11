#!/usr/bin/env bash
if [[ "$1" = "dev" ]]; then
    git clone git@github.com:cuttle-system/cuttle-vm.git
else
    git clone https://github.com/cuttle-system/cuttle-vm.git
fi
bash cuttle-vm/scripts/get-deps.sh