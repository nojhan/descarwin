if(( $# < 1 )); then
echo " ./$0 <#instances> <name of planner> <Save_ File.csv>"
exit
fi
rm _tmp*
echo -n "$2">>$3
for (( j = 1 ; j<= $1 ;j++))
do
   if (( $j < 10 )); then  motif=Res0$j/plan0$j
   else  motif=Res$j/plan$j
   fi 
    for (( i = 1 ; i <= 11 ; i++))
    do
	plan=$motif\_$i.soln
	if [  -f $plan ]; then makespan=`less $plan | grep ';Makespan' | awk '{print $3}'`
                      echo   "$makespan " >> _tmp
	fi
    done
   if [  -f _tmp ]; then sort -n _tmp > _tmp_sort 
    for line in `cat _tmp_sort`; 
    do 
	echo -n ";$line" >>$3
	break 
    done
   rm _tmp*
   else 
   echo -n ";0 ">>$3
   fi 
done
echo >> $3
