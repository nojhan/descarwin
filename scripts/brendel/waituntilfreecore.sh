numberofparralellprocesses=8
sleeptime=1
processname="dae"


if [ $# -gt 0 ] ; then
numberofparralellprocesses=$1
fi
if [ $# -gt 1 ] ; then
sleeptime=$2
fi
if [ $# -gt 2 ] ; then
processname=$3
fi

#echo $processname

n=`ps -e | grep -w $processname | wc -l`
#echo "number of processes " $n
while [ $n -ge $numberofparralellprocesses ]	
	do
	sleep $sleeptime
	n=`ps -e | grep -w $processname | wc -l`;
	#echo "number of processes " $n
done
