mkdir SOURCE
mv SOURCE.zip SOURCE
cd SOURCE
unzip SOURCE.zip >> /dev/null 2>&1
mkdir build
cd build
echo Building CPP Part -------------------------------------------------------------
cmake -D CMAKE_C_COMPILER=/opt/rh/devtoolset-8/root/usr/bin/gcc -D CMAKE_CXX_COMPILER=/opt/rh/devtoolset-8/root/usr/bin/g++ -D CMAKE_BUILD_TYPE=Release ..
make -j4
echo CPP Build Complete ------------------------------------------------------------
echo Building Java Part ------------------------------------------------------------
cd ../extfetcher
mkdir -p build/production/extfetcher
cd build
javac -d ./production/extfetcher -cp .:../libs/\*: ../src/*.java
cp ../src/*.xml ./production/extfetcher/
echo Java Build Complete -----------------------------------------------------------
