#!/bin/sh

source ./config.sh

here=$(pwd)

echo "Get source code of Descarwin"
echo "    archive"
cd $descarwin_git
git archive --prefix=descarwin/ --format zip master > descarwin.zip || exit
mv descarwin.zip $here
cd $here
if [[ -d descarwin ]] ; then
    rm -rf descarwin/
fi
echo "    unzip"
unzip -q descarwin.zip
rm -f descarwin.zip

echo "Get source code of ParadisEO"
echo "    archive"
cd $paradiseo_git
git archive --prefix=paradiseo/ --format zip master > paradiseo.zip || exit
# put the Paradiseo source code in the source directory also
mv paradiseo.zip $here/descarwin/
cd $here/descarwin/
echo "    unzip"
unzip -q paradiseo.zip

echo "Clean sources"
rm -rf doc/
rm -rf dae/doc/
rm -rf learnandoptimize/
rm -rf instances/
rm -rf scripts/
rm -rf sge/
rm -rf VAL-4.2.09.tar.gz
rm -rf paradiseo.zip


for p in $planners ; do
    for v in $tracks ; do
        echo "Prepare $v-$p"
        cd $here

        adir="$here/$v-$p"
        rm -rf $adir
        cp -r descarwin/ $adir
        cp build__$v-$p $adir/build
        cd $adir

        cat install-dist.cmake | sed "s,<< PATH_TO_OPEN_MPI_STATIC >>,$adir/openmpi," > install.cmake
        cat dae/install-dist.cmake | sed "s,<<PATH_TO_PARADISEO>>,$adir/paradiseo," > dae/install.cmake

    done # tracks
done # planners

