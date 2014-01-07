basedirectory=`pwd`
#cd descarwin/trunk
#rm -r -f dae
#svn checkout --username brendel https://scm.gforge.inria.fr/svn/descarwin/trunk/dae
#rm -r -f cpt
#svn checkout --username brendel https://scm.gforge.inria.fr/svn/descarwin/trunk/cpt
#cd cpt
#mkdir lib
#cd src
#make
#cd $basedirectory

#cd eo
#cmake -DBUILD_SHARED_LIBS=OFF .
#cmake .
#make
#cp -r lib release

#cd $basedirectory

cp install.cmake descarwin/trunk/dae/install.cmake
cd descarwin/trunk/dae
cmake -DBUILD_SHARED_LIBS=OFF . 
make

