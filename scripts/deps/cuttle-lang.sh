#!/usr/bin/env bash
if [[ "$1" -eq "dev" ]]; then
    git clone git@github.com:cuttle-system/cuttle-lang.git
else
    git clone https://github.com/cuttle-system/cuttle-lang.git
fi
bash cuttle-lang/scripts/get-deps.sh