#!/bin/bash

now=$(date +%s)
cd `pwd`
buildpath="build"
if [ ! -d $buildpath ]; then
    mkdir ${buildpath}
fi
type="DEBUG"
if [ $# == 1 ] && [ $1 == "R" ]; then
    type="RELEASE"
fi
if [ $# == 1 ] && [ $1 == "RD" ]; then
    type="RELWITHDEBINFO"
fi

cd ${buildpath}
rm CMakeCache.txt 2>/dev/null
cmake ../ -DLINUX=ON -DCMAKE_BUILD_TYPE=${type}
make -j3

cp ../Thridlibrary/linux/*.so.* linux/
cd linux
corepath="core"
if [ ! -d ${corepath} ]; then
    echo "exec $buildpath"
    ln -s ../../Config/core ${corepath}
fi

offset=$(($(date +%s)-${now}))
printf "runing time: %d min %d s\n\n" $((${offset}/60)) $((${offset}%60)) 

