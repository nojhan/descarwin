
dirtoconvert=$1

convert=convert.sh
echo "" > $convert

# convert to ascii
for ext in {h,cpp}; do
    find $dirtoconvert -name "*.${ext}" -exec file {} \; | awk -F ":" '{print $1 $2;}'| awk '{print "iconv -f "$2" -t ASCII//TRANSLIT -o "$1".ascii "$1;}'|sed "s/ISO-8859/ISO88591/" >> $convert
done

bash ./convert.sh


# remove comments and empty lines
clean=clean.sh
echo "" > $clean

for ext in {h,cpp}; do
    find . -name "*.ascii" -exec echo "sed -nf /home/nojhan/code/descarwin/trunk/scripts/remove_comments.sed {} > {}.nocom ; sed '/^\s*$/d' {}.nocom > {}.ok" \; >> $clean
done

bash ./clean.sh


