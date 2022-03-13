mkdir SOURCE
mv SOURCE.zip SOURCE
cd SOURCE
unzip SOURCE.zip >> /dev/null 2>&1
mkdir build
cd build
cmake -D CMAKE_C_COMPILER=/opt/rh/devtoolset-8/root/usr/bin/gcc -D CMAKE_CXX_COMPILER=/opt/rh/devtoolset-8/root/usr/bin/g++ -D CMAKE_BUILD_TYPE=Release ..
make -j4
