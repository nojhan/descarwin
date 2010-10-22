
base_dir=$1

cd $base_dir

today=`date --iso-8601`

# make a directory of the current date
mkdir $today
cd $today

here=`pwd`

# archive current version of EO
cd ~/code/eodev/eodev/
git archive --format zip master > eo_${today}.zip
cp eo_${today}.zip $here

# archive current version of DAE
cd ~/code/descarwin/
svn export dae/ dae_${today}
zip -r dae_${today}.zip dae_${today}
cp dae_${today}.zip $here

# copy the binaries
cp dae/build/dae $here
cp DAEx1.0/DAEx/DAEx $here/dae_jack
cp DAEx1.0/DAEx/parametre $here

# copy scripts that manages the campaign
cp dae/scripts/* $here

cd $here

touch README

