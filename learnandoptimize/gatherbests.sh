echo -n >bestlogs.txt
echo -n >bestlogs_unique.txt

cat log_*.txt | grep -v runningtime > alllogs.txt

for f in `ls ./log_*.txt`
    do
    #echo $f
    awk -f gatherbest.awk $f >>bestlogs.txt
    awk -f gatherbest_unique.awk $f >>bestlogs_unique.txt

    done

echo "Default CMA-ES Genetransferer ANN"> bestlog_statistics.txt

grep  initial bestlogs.txt | wc -l | tr -d '\012' >> bestlog_statistics.txt
echo -n " ">> bestlog_statistics.txt 
grep  CMA bestlogs.txt | wc -l | tr -d '\012'>> bestlog_statistics.txt
echo -n " ">> bestlog_statistics.txt 
grep  gene bestlogs.txt | wc -l | tr -d '\012'>> bestlog_statistics.txt
echo -n " ">> bestlog_statistics.txt 
grep  ANN bestlogs.txt | wc -l | tr -d '\012'>> bestlog_statistics.txt

echo >> bestlog_statistics.txt

grep  initial bestlogs_unique.txt | wc -l | tr -d '\012'>> bestlog_statistics.txt
echo -n " ">> bestlog_statistics.txt 
grep  CMA bestlogs_unique.txt | wc -l | tr -d '\012'>> bestlog_statistics.txt
echo -n " ">> bestlog_statistics.txt 
grep  gene bestlogs_unique.txt | wc -l | tr -d '\012'>> bestlog_statistics.txt
echo -n " ">> bestlog_statistics.txt 
grep  ANN bestlogs_unique.txt | wc -l | tr -d '\012'>> bestlog_statistics.txt

