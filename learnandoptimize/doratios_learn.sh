cat log_*.txt | grep default  > defaultlogs.txt
cat defaultlogs.txt bestlogs_unique.txt | grep -v agregatedmakespan > defaultandbestlogs.tmp
sort -k1,1g defaultandbestlogs.tmp > defaultandbestlogs.txt
rm defaultandbestlogs.tmp
awk -f quality_ratio_lao.awk defaultandbestlogs.txt >qualityratios.txt