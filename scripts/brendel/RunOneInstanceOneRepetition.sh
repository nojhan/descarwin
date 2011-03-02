if(( $# < 4 )); then
echo " ./$0 <domain file basename> <problem file basename> <#instance> <# of this repetition> <duration in sec>"
exit
fi

j=$3
i=$4



    mkdir Res$j
    
    #echo $# $5

    if [ $# -gt 4 ] && [ $5 -gt 0 ]; then

    echo "time constraint: $5 seconds" 
    
    if (( $j < 10 )); then 
       nohup DAEx -o ${1}0$j-domain.pddl -f ${2}0$j.pddl --resDir=Res$j/res$i --fileResult=Res${j}/plan${j}_$i.soln&
    else
    nohup DAEx -o $1$j-domain.pddl  -f $2$j.pddl --resDir=Res$j/res$i --fileResult=Res${j}/plan${j}_$i.soln&
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
	
	 if (( $j < 10 )); then  
	DAEx -o ${1}0$j-domain.pddl -f ${2}0$j.pddl --resDir=Res$j/res$i --fileResult=Res${j}/plan${j}_$i.soln 
	else 
	DAEx -o $1$j-domain.pddl  -f $2$j.pddl --resDir=Res$j/res$i --fileResult=Res${j}/plan${j}_$i.soln
	fi 
	             
    fi

    
    
     less Res${j}/plan${j}_$i.soln | grep 'Makespan' | awk '{print $3}' > Res$j/makespan${j}_$i.soln
     less Res${j}/plan${j}_$i.soln | grep 'Totalcost' | awk '{print $3}' > Res$j/makespan${j}_$i.soln
     
#     rm Res${j}plan${j}_$i.soln
             
    


