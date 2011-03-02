iterationsperparameter=11;
numberofcores=7;
sleeptime=3;

if(( $# < 2 )); then
echo " ./$0 <number of first jobs to monitor> <number of last job to monitor> <number of cores>"
exit
fi


if(( $# > 2 )); then
numberofcores=$3
echo "number of cores changed to " $numberofcores
fi

while [ 1 ] 
do

for (( i = $1; i <= $2; i++))
do
#echo $i

bash waituntilprocfree.sh $numberofcores $sleeptime "DAEx"

if [ -f workers_exit.txt ]
    then
    echo "workers exit"
    exit
    fi

if [ -f ./instances/$i/parameters_$i.ready ];
then
    rm ./instances/$i/parameters_$i.ready
    echo catched ./instances/$i/parameters_$i
    iterationnumber=`tail -1 ./instances/$i/parameters_$i`	
    instancenumber=`tail -2 ./instances/$i/parameters_$i | head -1`
    directory=`tail -3 ./instances/$i/parameters_$i | head -1`

    echo $directory $instance $iterationnumber
    cp ./instances/$i/parameters_$i ${directory}parametre_$i
	

    if (( $iterationnumber < $iterationsperparameter)); then
        bash waituntilprocfree.sh $numberofcores $sleeptime "DAEx"
	nohup bash ./learnoptimizeworker_core.sh $directory $instancenumber $iterationnumber $i&	
	sleep $sleeptime  #this is just to slow things down a little bit so that waituntilfreecore.sh is reliable
    else
    for (( j=0; j<iterationsperparameter; j++))
        do
        if [ -f workers_exit.txt ]
            then
            exit
            fi
            bash waituntilprocfree.sh $numberofcores $sleeptime "DAEx"
	nohup bash ./learnoptimizeworker_core.sh $directory $instancenumber $j $i&
	sleep $sleeptime  #this is just to slow things down a little bit so that waituntilfreecore.sh is reliable
    done #for j
    fi #else
    
    sleep 30 #we have taken a job, let us not take another for a while!
fi #if -f

sleep 1

done # for i


done # while 

