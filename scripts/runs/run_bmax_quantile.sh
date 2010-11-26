#!/bin/bash

# no core dump
ulimit -c 1

domain=$1
instance=$2
options=$3
res=$4
quantile=$5
q=0.$q

#r=$SGE_TASK_ID
r=2
s=$((`date +%s`/r))

#d=`basename $domain .pddl`
d=`echo $domain | cut -d '/' -f 7`
i=`basename $instance .pddl`

echo "Run dae on $d : $i, run $r, b_max quantile $q" 1>&2

cmd="./dae --max-seconds=1799 --domain=$domain --instance=$instance --seed=$s --status=$res/status/dae_status.${d}_${i}_${q}_r$r --sequential=$3 --plan-file=$res/plans/dae_plan_${d}_${i}_${q}_r$r --bmax-quantile=${q}"


echo "  $cmd" 1>&2

$cmd

