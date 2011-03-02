#./gatherrrunningtimedata.sh >runningtimes_11.txt
awk -f medianrunningtimes.awk runningtimes_11.txt >runningtimes.txt
cp runningtimes.txt runningtimes_all.txt
awk -f sample.awk runningtimes_all.txt
cp train.txt runningtimes.txt