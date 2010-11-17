#This script supposes that we are in a root directory where descarwin, eo and cpt is downloaded

mv eo paradiseo-eo
cd paradiseo-eo
cmake -DBUILD_SHARED_LIBS=OFF .
cmake .
make
mkdir build
cp -r lib build/lib
cd ..
cd CPT
mkdir lib
cd src
make
cd ../..
cp install.cmake descarwin/trunk/dae/install.cmake
cd descarwin/trunk/dae
cmake -DBUILD_SHARED_LIBS=OFF . 
make

