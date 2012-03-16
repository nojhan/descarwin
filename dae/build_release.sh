#!/bin/sh

mkdir -p release
cd release/
cmake .. -DCMAKE_BUILD_TYPE=Release && make -j 2

