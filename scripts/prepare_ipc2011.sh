
today=`date --iso-8601`
# make a directory of the current date
mkdir $today
cd $today

mkdir seq-sat-DAE_YAHSP
cd seq-sat-DAE_YAHSP 

here=`pwd`

echo "archive current version of EO"
cd ~/code/eodev/eodev/eo
mkdir -p $here/eo
git archive --format zip master > $here/eo/eo_${today}.zip
cd $here/eo
unzip -qq eo_${today}.zip
cd $here

echo "archive current version of DAE/YAHSP"
svn export ~/code/descarwin/trunk/ $here/dae_yahsp

echo "copy the IPC build file"
cp ~/code/descarwin/trunk/scripts/build $here

cd ..

echo "duplicate seq to tempo"
cp -r seq-sat-DAE_YAHSP tempo-sat-DAE_YAHSP

