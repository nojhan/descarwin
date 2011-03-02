first=0
llast=64


if(( $# > 0 )); then
first=$1
fi

if(( $# > 1 )); then
llast=$2
fi




directory=`pwd`;

echo "starting workers in " $directory > workers.txt

for (( i=${first}; i<${llast}; i++ )) 
do
echo $i

sleep 5


	if (( $i < 10 )); then
		h=tipi0$i

	else
		h=tipi$i
	fi

	if (( $i < 48 )); then
		n=4

	else
		n=8
	fi

if [ -f worker_$h.log ]; then
    echo $i already running
else
    echo $i not yet running

    count=`ping -c 1 $h | grep 'received' | awk -F',' '{ print $2 }' | awk '{ print $1 }'`

    echo $count

    if (( $count > 0 )); then

	ssh -o ConnectTimeout=10 $h $directory/startworkers_core.sh $directory $n;
    fi
fi


done

