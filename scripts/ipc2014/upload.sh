#!/bin/bash

source ./config.sh

for p in $planners ; do
    for v in $tracks ; do
        rm -rf $v-$p/paradiseo/release/
        rm -rf $v-$p/release/
        rm -rf $v-$p/dae_yahsp
        rsync -az --progress $v-$p ipc_jxd@ipc2014.hud.ac.uk:/home/ipc_jxd/
    done
done

