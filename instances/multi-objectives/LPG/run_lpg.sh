#!/bin/bash

lim=$1
dom=$2
prob=$3
dir=$4

tmp=tmp.pddl

loop=1
mkdir -p $dir

start=$(date +"%s")

while true; do
    for ((a=0; a<=10; a++)); do
	cp $prob $tmp
	echo "(:metric minimize (+ (* $a (total-time)) (* $((10-a)) (total-cost)))))" >> $tmp
	res=$dir/plan_L$(printf "%02d" $loop)-A$(printf "%02d" $a)
	time=$(date +"%s")
	(ulimit -t $lim && bin/lpg-td -o $dom -f $tmp -n 2 -out $res+$((time-start))+ -repeats 1000000 > /dev/null)
    done
    ((loop++))
done

rm $tmp