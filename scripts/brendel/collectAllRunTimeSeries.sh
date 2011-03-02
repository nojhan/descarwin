if(( $# < 1 )); then
echo " ./$0 <#instances>"
exit
fi
for (( j = 1 ; j<= $1 ;j++))
do
   if (( $j < 10 )); then  d=Res0$j
   else  d=Res$j
   fi 
    for (( i = 1 ; i <= 1 ; i++))
    do
	bestxg=$d/res$i/best.xg
	echo -n "\""
	echo -n $bestxg
	echo -n "\","
	if [  -f $bestxg ]; then less $bestxg | awk '{print $4; print ","}'| tr -d '\012'
	fi
	echo 
    done
done
