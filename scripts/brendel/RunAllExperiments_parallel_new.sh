#! /bin/bash 

numberofparralellprocesses=7;
sleeptime=10;


if(( $# < 5 )); then
echo " ./$0 <domain file basename> <problem file basename> <#first instance> <#last instance> <#repetitions> <duration in sec> <numberofparralellprocesses>"
exit
fi

if(( $# > 6 )); then
numberofparralellprocesses=$7
echo "number of processes changed to " $numberofparralellprocesses
fi

if(( $# > 7 )); then
sleeptime=$8
echo "sleeptime changed to " $sleeptime
fi



if (( $3 > 0 )); then

	for (( j = $3 ; j<= $4 ;j++))
		do
		rm -r Res$j
		mkdir Res$j
		for (( i = 1; i <= $5; i++))
		    	do
			waituntilprocfree.sh $numberofparralellprocesses $sleeptime
			echo "RunOneInstanceOneRepetition_new.sh " $1 $2 $j $i $6
			nohup bash RunOneInstanceOneRepetition_new.sh  $1 $2 $j $i $6&
			sleep $sleeptime
			
			done # for i
		done # for j
else

	for j in `less train.txt`
	do
		rm -r Res$j
		mkdir Res$j
		for (( i = 1; i <= $5; i++))
		    	do
			waituntilprocfree.sh $numberofparralellprocesses $sleeptime
			echo "RunOneInstanceOneRepetition_new.sh " $1 $2 $j $i $6
			nohup bash RunOneInstanceOneRepetition_new.sh  $1 $2 $j $i $6&
			sleep $sleeptime
			
			done # for i

	done

fi

sleep $6 #we have to wait for the last one to finish

sleep 10 #still wait a little bit, for shure

#sleep 100 #we have to wait for the last one to finish
