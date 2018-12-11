#!/usr/bin/env bash

cd $(dirname "$0")
SCRIPTS_PATH=$(pwd)
cd ../..

for dep in $SCRIPTS_PATH/deps/*
do
    echo Getting dependency: $dep
    bash $dep $1
done