curdir=`pwd`;

for ((i=14;i<21;i++))
do

daex_revac.sh   $curdir $i $i 2000 30 11

cp revac.log revac_$i.log


done


