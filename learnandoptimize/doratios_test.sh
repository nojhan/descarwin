cat log_*.txt  > alllogs.txt
cat log_*.txt | grep ANN  > alllogs.tmp
cat defaultlogs.txt alllogs.tmp > alllogs.txt
rm alllogs.tmp
sort -k1,1g alllogs.txt >alllogs_sorted.txt

awk -f quality_ratio.awk alllogs_sorted.txt >qualityratios.txt