#!/bin/bash

lim=$1
dom=$2
prob=$3
dir=$4
runNb=$5 # le numéro du run, pour servir de graine aléatoire

tmp=tmp.pddl

loop=1
mkdir -p $dir

start=$(date +"%s")
log=$lim.$dom.$prob.$dir.$run.log
echo GO2 > $log
while true; do
    for ((a=0; a<=10; a++)); do
	cp $prob $tmp
	echo "(:metric minimize (+ (* $a (total-time)) (* $((10-a)) (total-cost)))))" >> $tmp
	res=$dir/plan_L$(printf "%02d" $loop)-A$(printf "%02d" $a)
	time=$(date +"%s")
seed=$loop
echo "ulimit -t $lim && bin/lpg-td -o $dom -f $tmp -n 2 -seed $seed -out $res+$((time-start))+ -repeats 1000000"  >> $log
	(ulimit -t $lim && bin/lpg-td -o $dom -f $tmp -n 2 -seed $seed -out $res+$((time-start))+ -repeats 1000000 > /dev/null)
    done
    ((loop++))
done

rm $tmp
