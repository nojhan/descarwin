if(( $# < 7 )); then 
echo " ./$0 <domain folder> <first problem> <last problem> <number of revac iterations> <time for daex> <number of daex iterations <statistical method>"
exit
fi

matlab.sh "setpath; daex_revac('$1',$2,$3,$4,$5,$6,'$7');exit;"

