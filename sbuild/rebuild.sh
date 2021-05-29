#!/bin/bash

if [ -x sbuild/cycc1 ]; then
    echo 'exists cycc1'
else
    exit 1
fi

cd sbuild/
make -f Makefile2

