#!/usr/bin/env bash
if [[ "$1" = "dev" ]]; then
    git clone git@github.com:cuttle-system/cuttle-translator.git
else
    git clone https://github.com/cuttle-system/cuttle-translator.git
fi
bash cuttle-translator/scripts/get-deps.sh