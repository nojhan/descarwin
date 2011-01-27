
eo_dir=/tools/eo/eo/
eo_git=0
descarwin_trunk=~/TGV/DESCARWIN/dae/descarwin/trunk/

today=`date --iso-8601`
# make a directory of the current date
mkdir $today
cd $today

mkdir seq-sat-dae_yahsp
cd seq-sat-dae_yahsp 

here=`pwd`

echo "archive current version of EO"
if [ eo_git == 1 ] ; then
    cd $eo_dir
    mkdir -p $here/eo
    git archive --format zip master > $here/eo/eo_${today}.zip
    cd $here/eo
    unzip -qq eo_${today}.zip
    cd $here
else
    cp -r $eo_dir $here/eo
fi

echo "archive current version of DAE/YAHSP"
svn export $descarwin_trunk $here/daeyahsp

echo "clean DAE code comments"
cd ..
cp $descarwin_trunk/scripts/remove_comments.sed .
$descarwin_trunk/scripts/clean_code.sh $here/daeyahsp/dae

echo "copy the IPC build and plan file"
cp $descarwin_trunk/scripts/build $here
cp $descarwin_trunk/scripts/plan $here

cd $here/..

echo "duplicate seq to tempo"
cp -r seq-sat-dae_yahsp tempo-sat-dae_yahsp

echo "archive everything"
tar cjf dae_yahsp.tar.bz2 seq-sat-dae_yahsp tempo-sat-dae_yahsp

