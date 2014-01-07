#!/bin/sh

paradiseo_git="/home/nojhan/code/paradiseo"
descarwin_git="/home/nojhan/code/descarwin"

planners="dae_yahsp"
# planners="dae_yahsp daed_yahsp"
tracks="seq-sat tempo-sat seq-mco"

here=$(pwd)

echo "Get source code of Descarwin"
cd $descarwin_git
git archive --prefix=descarwin/ --format zip master > descarwin.zip || exit
mv descarwin.zip $here
cd $here
if [[ -d descarwin ]] ; then
    rm -rf descarwin/
fi
unzip descarwin.zip

echo "Get source code of ParadisEO"
cd $paradiseo_git
git archive --prefix=paradiseo/ --format zip master > paradiseo.zip || exit
# put the Paradiseo source code in the source directory also
mv paradiseo.zip $here/descarwin/
cd $here/descarwin/
unzip paradiseo.zip


for p in $planners ; do
    for v in $tracks ; do
        echo "Prepare $v-$p"
        cd $here

        adir="$here/$v-$p"
        cp -r descarwin/ $adir
        cp build__$v-$p $adir/build
        cd $adir

        cat install-dist.cmake | sed "s,<< PATH_TO_OPEN_MPI_STATIC >>,$adir/openmpi," > install.cmake
        cat dae/install-dist.cmake | sed "s,<<PATH_TO_PARADISEO>>,$adir/paradiseo," > dae/install.cmake

    done # tracks
done # planners

cd $here
for p in $planners ; do
    for v in $tracks ; do
        echo "Build $v-$p"
        cd $here/$v-$p
        ./build
    done # tracks
done # planners

