#!/bin/bash

# no core dump
ulimit -c 1

domain=$1
instance=$2
res=$3

r=$SGE_TASK_ID
#r=2
s=$((`date +%s`/r))

d=`echo $domain | cut -d '/' -f 7`
i=`basename $instance .pddl`

echo "Run test b_max on $d : $i, run $r" 1>&2

cmd="./test_b_max --domain=$domain  --instance=$instance --runs=1 --seed=$s"


echo "  $cmd" 1>&2

$cmd

