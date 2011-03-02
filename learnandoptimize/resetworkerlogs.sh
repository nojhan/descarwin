cp empty.log learnoptimize.log
for f in `ls worker_*.log`
do
echo $f
echo -n  > $f
done
