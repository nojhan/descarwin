rm revac_allresults.txt

curdir=`pwd`;

for ((i=14;i<21;i++))
do

grep "daex run" ./revac_$i.log >revac_${i}_allruns.log
grep "accumulated" ./revac_$i.log >revac_${i}_accumalatedonly.log

sort revac_${i}_allruns.log -g -k14 > revac_${i}_allruns_sorted.log
sort revac_${i}_accumalatedonly.log -g -k11 > revac_${i}_accumalatedonly_sorted.log

less revac_${i}_accumalatedonly.log | awk '{if ($11<1000000) print $11}' > revac_${i}_accumalatedonly_resultsonly.log


echo -n $i " " >> revac_allresults.txt

less revac_${i}_allruns_sorted.log | head -1 | awk '{print $14}'>> revac_allresults.txt

done


