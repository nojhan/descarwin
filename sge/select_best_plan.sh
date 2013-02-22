#!/bin/bash

if [ $# = 1 ]
then
    REP=$1;
else
    echo "Usage: $0 directory/in/which/are/the/plan.soln/files/"
    echo "Don't forget the last slash /"
    exit 0;
fi

grep -H Makespan ${REP}plan.soln* | awk '{ print $3 " " $1 }' | sort -n | head -n1

