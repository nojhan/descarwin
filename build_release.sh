#!/usr/bin/env sh
mkdir -p release
cd release
cmake -DCPT_WITH_OMP=1   ..
make
cd ..
