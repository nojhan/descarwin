directory=$1
numproc=$2
h=`hostname`;

if [ -d $directory ]
then

. ./bashrc.sh
cd $directory
echo $h $numproc >>workers.txt;

./startworker.sh $h 0 600 $numproc


fi
