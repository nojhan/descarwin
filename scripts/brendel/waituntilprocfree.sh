numberofprocessors=7
sleeptime=1
processname="dae"


if [ $# -gt 0 ] ; then
numberofprocessors=$1
fi

if [ $# -gt 1 ] ; then
sleeptime=$2
fi

if [ $# -gt 2 ] ; then
processname=$3
fi


#let limit=100-100/$numberofprocessors;


#echo limit $limit
#echo numberofprocessors $numberofprocessors
#echo sleeptime $sleeptime


n=`ps -e | grep -w $processname | wc -l`

#echo numproc $n

#x=`top -b -n1 -d 1 | grep Cpu | awk '{print $2}' | awk 'BEGIN { FS = "%" } ; {print $1}' | awk 'BEGIN { FS = "." } ; {print $1}' | awk 'BEGIN { FS = " "};{print $1}'`; 

x=`less /proc/loadavg | awk 'BEGIN { FS = " " } ; {print $1}' | awk 'BEGIN { FS = "." } ; {print $1}'`

#echo cpu $x

while true  
	do
	
#	echo cpu $x	
#	echo numproc $n

	if (( $x < $numberofprocessors && $n < $numberofprocessors ))
		then
		exit
		fi  

	if [ -f workers_exit.txt ]
	    then
	    exit
	    fi
	            
	sleep $sleeptime
	#x=`top -b -n2 -d 1 | grep Cpu | awk '{print $2}' | awk 'BEGIN { FS = "%" } ; {print $1}' | awk 'BEGIN { FS = "." } ; {print $1}' | awk 'BEGIN { FS = " " };{print $2}'`; 
	x=`less /proc/loadavg | awk 'BEGIN { FS = " " } ; {print $1}' | awk 'BEGIN { FS = "." } ; {print $1}'`
	n=`ps -e | grep -w $processname | wc -l`
	
	done

