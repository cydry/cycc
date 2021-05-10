#!/bin/bash

if [ -x cycc ]; then
    cp cycc sbuild/
    cp *.h sbuild/
    cp *.c sbuild/
    rm sbuild/helper.c
else
    exit 1
fi

cd sbuild/
make

