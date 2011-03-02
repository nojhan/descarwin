#! /bin/bash


if(( $# < 4 )); then
echo " ./$0 <directory> <instance> <iteration> <index for output file>"
exit
fi

WAIT=3000;

echo $1 $2 $3 $4


j=$2
i=$3

basedir=`pwd`

outdir=$basedir/instances/$4

cd $1

rm plan${j}_$i.soln 

rm -r Res$j/res$i

mkdir Res$j
mkdir Res$j/res$i

	
	if (( $j < 10 )); then
		s=`grep durative   p0$j-domain.pddl | wc | awk '{print $1}'`
			if (($s > 0 )); then 
				echo "--seq=1" >> parametre_$4
			fi
		nohup DAEx -o p0$j-domain.pddl -f p0$j.pddl --fileResult=plan${j}_$i.soln --resDir=Res$j/res$i  @parametre_$4&
	else 
		s=`grep durative   p$j-domain.pddl | wc | awk '{print $1}'`
			if (($s > 0 )); then
				echo "--seq=1" >> parametre_$4
			fi
		nohup DAEx -o p$j-domain.pddl  -f p$j.pddl --fileResult=plan${j}_$i.soln --resDir=Res$j/res$i  @parametre_$4&
	fi 
	
	START=$(date +%s);
	DIFF=-1;
	   
        
	mypid=$!
        chkrunning=1
	for ((c=1 ; (DIFF<WAIT) && ($chkrunning>0) ; c++  ))
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
	
	t=0
	             
	while (( $t < 50 ))
	do	
	sleep 1
	t=$t+1
	if [ -s plan${j}_$i.soln ] 
	then
	    break
	fi
	done
	
	sleep 5
	
	 less plan${j}_$i.soln | grep ';Makespan' | awk '{print $3}' > $outdir/result_$4_$i.txt
	 
        if [ -s $outdir/result_$4_$i.txt ]; then
    	    echo "OK"
        else
	    echo "parameters_$i.ready does not exist, trying best.xg"
	    tail -n1 Res$j/res$i/best.xg | awk '{print $4}' > $outdir/result_$4_$i.txt
	fi
     

             
cd $basedir

hostname > $outdir/result_$4_$i.ready
