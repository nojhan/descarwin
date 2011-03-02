#! /bin/bash

if(( $# < 4 )); then
echo " ./$0 <domain file > <problem > <resultfile> <duration in sec>"
exit
fi


    if [ $# -gt 3 ] && [ $4 -gt 0 ]; then

    echo "time constraint: $4 seconds" 
    
   nohup DAEx -o $1 -f $2  --resDir=resdir --fileResult=$3&

    
    START=$(date +%s)
    DIFF=-1
    
    
    #echo $START
    #echo $5
    
        
	mypid=$!
        chkrunning=1
	for ((c=1 ; (DIFF<$4) && ($chkrunning>0) ; c++  ))
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
	
	DAEx -o $1 -f $2  --resDir=resdir --fileResult=$3
	
	             
    fi



