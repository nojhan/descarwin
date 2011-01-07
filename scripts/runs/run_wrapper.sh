#!/bin/bash

# no core dump
ulimit -c 1

domain=$1
instance=$2
res=$3
seq=$4

r=$SGE_TASK_ID
#r=2
s=$((`date +%s`/r))

#d=`basename $domain .pddl`
d=`echo $domain | cut -d '/' -f 7`
i=`basename $instance .pddl`

echo "Run dae on $d : $i, run $r" 1>&2

cmd="./dae --max-seconds=1799 --domain=$domain --instance=$instance --seed=$s --status=$res/status/dae_status.${d}_${i}_r$r --plan-file=$res/plans/dae_plan_${d}_${i}_r$r --runs-max=0"

echo "  $cmd" 1>&2

$cmd

filename="$res/plans/dae_plan_${d}_${i}_r$r"
lastfile=`ls -1 -v $filename* | tail -n 1`
cp $lastfile $filename

if [ "$seq" = 0 ] ; then 
    cmd="./validate -t 0.00001 $domain $instance $filename"
else 
    cmd="./validate $domain $instance $filename"
fi

echo "  $cmd" 1>&2

$cmd
