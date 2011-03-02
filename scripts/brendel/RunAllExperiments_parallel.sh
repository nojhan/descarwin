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


if (( $3 > 0 )); then

	for (( j = $3 ; j<= $4 ;j++))
		do
		rm -r Res$j
		mkdir Res$j
		for (( i = 1; i <= $5; i++))
		    	do
			
			n=`ps -e | grep DAEx | wc -l`;
			echo "number of processes " $n
			while [ $n -ge $numberofparralellprocesses ]	
				do
				sleep $sleeptime
				n=`ps -e | grep DAEx | wc -l`;
				echo "number of processes " $n
				done
			echo "RunOneInstanceOneRepetition.sh" $1 $2 $j $i $6
			nohup bash RunOneInstanceOneRepetition.sh $1 $2 $j $i $6&
			sleep 1
			
			done # for i
		done # for j
else

	for j in `less train.txt`
	do
		rm -r Res$j
		mkdir Res$j
		for (( i = 1; i <= $5; i++))
		    	do
			
			n=`ps -e | grep DAEx | wc -l`;
			echo "number of processes " $n
			while [ $n -ge $numberofparralellprocesses ]	
				do
				sleep $sleeptime
				n=`ps -e | grep DAEx | wc -l`;
				echo "number of processes " $n
				done
			echo "RunOneInstanceOneRepetition.sh" $1 $2 $j $i $6
			nohup bash RunOneInstanceOneRepetition.sh $1 $2 $j $i $6&
			sleep 1
			
			done # for i

	done

fi

