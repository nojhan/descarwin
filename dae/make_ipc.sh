#!/bin/sh

mkdir -p ipc
cd ipc
cmake .. -DCMAKE_BUILD_TYPE=Release && make -j 4 dae

