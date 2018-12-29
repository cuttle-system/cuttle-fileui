#!/usr/bin/env bash
if [[ "$1" = "dev" ]]; then
    git clone git@github.com:cuttle-system/cuttle-parser.git
else
    git clone https://github.com/cuttle-system/cuttle-parser.git
fi
bash cuttle-parser/scripts/get-deps.sh $1