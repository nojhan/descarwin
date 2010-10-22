#!/bin/bash

# no core dump
ulimit -c 1

domain=$1
instance=$2
options=$3
res=$4

r=$SGE_TASK_ID
#r=2
s=$((`date +%s`/r))

#d=`basename $domain .pddl`
d=`echo $domain | cut -d '/' -f 7`
i=`basename $instance .pddl`

echo "Run dae_jack on $d : $i, run $r" 1>&2

cmd="./dae_jack --o=$domain --f=$instance --seq=$options --seed=$s --resDir=$res/gens/jack_gens_${d}_${i}_r${r} --fileResult=$res/plans/jack_plan_${d}_${i}_r${r} --fake "

# le fake est nécessaire à cause d'un bug étrange : le dernier paramètre n'est pas pris en compte.
# FIXME the status option does not seems to work
#--status=status/status_jack.${d}_${i}_r$r"


echo "  $cmd" 1>&2

$cmd
