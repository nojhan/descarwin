less $1 | grep "accumulated makespan" | awk '{if ($4==0) print " " ; print $3 " " $4 " " $5}'
