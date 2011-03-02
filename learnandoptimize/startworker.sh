

if(( $# < 2 )); then
echo " ./$0 <number of computer> <number of first jobs to monitor> <number of last job to monitor> <number of processors>"
exit
fi

nohup bash ./learnoptimizeworker.sh $2 $3 $4 > worker_$1.log&



