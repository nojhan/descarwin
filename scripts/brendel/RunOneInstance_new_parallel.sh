numberofparralellprocesses=7;
sleeptime=10;

if(( $# < 4 )); then
echo " ./$0 <domain file basename> <problem file basename> <#instance> <#repetitions> <duration in sec> <output dir (optional)>"
exit
fi

j=$3

if (( $#>5 )); then
mkdir $6
mkdir $6/Res$j
outputdir="$6/Res$j"
else
    mkdir Res$j
    outputdir=Res$j
    
fi
    
    
    for (( i = 1; i <= $4; i++))
    do
    	
    #echo $# $5

    if [ $# -gt 4 ] && [ $5 -gt 0 ]; then

    echo "time constraint: $5 seconds" 

    waituntilfreecore.sh $numberofparralellprocesses  $sleeptime
    
    if (( $j < 10 )); then 
    nohup dae -D=${1}0$j-domain.pddl -I=${2}0$j.pddl -q=0 > $outputdir/plan${j}_$i.soln&
    else
    nohup dae -D=$1$j-domain.pddl  -I=$2$j.pddl -g=0 > $outputdir/plan${j}_$i.soln&
    fi

    
    START=$(date +%s)
    DIFF=-1
    
    
    #echo $START
    #echo $5
    
        
	mypid=$!
        chkrunning=1
	for ((c=1 ; (DIFF<$5) && ($chkrunning>0) ; c++  ))
    	     do
	    sleep 1
	    chkrunning=$(ps | grep -c $mypid)
	    END=$(date +%s);
	    DIFF=$(( $END - $START ));
	    #echo "END:" $END
	    #echo "diff:" $DIFF
	    #echo "desired endtime:" $5
	    #echo "checkrunning:" $chkrunning
	    done
	

	if (($chkrunning>0)); then  
	
	for ((trykill=1; ($chkrunning>0) && (trykill<10) ; trykill++ ))
	    do
	    kill -9 $mypid
	    chkrunning=0;
	    chkrunning=$(ps  | grep -c $mypid)
	    #ps 
	    echo $mypid
	    done
        fi
    else
	echo "no time constraint"

       waituntilfreecore.sh $numberofparralellprocesses  $sleeptime
	
	 if (( $j < 10 )); then  
	dae -D=${1}0$j-domain.pddl -I=${2}0$j.pddl -q=0 > $outputdir/plan${j}_$i.soln 
	else 
	dae -D=$1$j-domain.pddl  -I=$2$j.pddl > $outputdir/plan${j}_$i.soln 
	fi 
	             
    fi

             
    

    done

