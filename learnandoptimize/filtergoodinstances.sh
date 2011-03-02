tail -n1 log_*.txt | sort -k5,5 -k4,4n | grep ./ > goodlogs.txt

awk  runningtimes.txt