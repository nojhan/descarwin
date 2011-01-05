#!/bin/bash

# no core dump
ulimit -c 1

domain=$1
instance=$2
res=$3

r=$SGE_TASK_ID
#r=2
s=$((`date +%s`/r))

i=`basename $instance .PDDL`

echo "Run dae: $i, run $r, out $res" 1>&2

cmd="./dae --max-seconds=10799 --domain=$domain --instance=$instance --seed=$s --status=$res/status/dae_status_${i}_r$r --plan-file=$res/plans/dae_plan_${i}_r$r"


echo "  $cmd" 1>&2

$cmd

