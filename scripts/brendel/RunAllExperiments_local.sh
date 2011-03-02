if(( $# < 5 )); then
echo " ./$0 <domain file basename> <problem file basename> <#first instance> <#last instance> <#repetitions> <duration in sec> <output dir (optional)>"
exit
fi

if (( $3 > 0 )); then

	for (( j = $3 ; j<= $4 ;j++))
		do
		RunOneInstance.sh $1 $2 $j $5 $6 $7
	done
else

	for j in `less train.txt`
		do
		RunOneInstance.sh $1 $2 $j $5 $6 $7
	done

fi

