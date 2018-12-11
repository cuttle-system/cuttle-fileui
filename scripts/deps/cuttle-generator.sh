#!/usr/bin/env bash
if [[ "$1" = "dev" ]]; then
    git clone git@github.com:cuttle-system/cuttle-generator.git
else
    git clone https://github.com/cuttle-system/cuttle-generator.git
fi
bash cuttle-generator/scripts/get-deps.sh