#!/bin/bash

source ./config.sh

cd $here
for p in $planners ; do
    for v in $tracks ; do
        echo "Build $v-$p"
        cd $here/$v-$p
        ./build
    done # tracks
done # planners


