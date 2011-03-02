domain=$1
problem=$2
features=$3

featureselector $domain $problem | grep features | tr -d '\012' > features_raw.txt
wc $domain | tr -d '\012'  >> features_raw.txt
wc $problem | tr -d '\012'  >> features_raw.txt
grep durative   $domain | wc | awk '{print " " $1}' | tr -d '\012' >>features_raw.txt

awk -f preprocessfeatures.awk features_raw.txt > $features






