#!/bin/bash

cd `pwd`
nowpath=`pwd`
binpath="bin"
if [ ! -d ${binpath} ]; then
    mkdir ${binpath}
fi

cd bin/
includepath="include"
if [ ! -d ${includepath} ]; then
    mkdir ${includepath}
fi

libpath="lib/linux"
if [ ! -d ${libpath} ]; then
    mkdir -p ${libpath}
fi
cd ${nowpath}
. ./make.sh
cd ${nowpath}
cp API/* ${binpath}/${includepath}/
cp Public/MultiSys.h ${binpath}/${includepath}/
cp Public/LinSys.h ${binpath}/${includepath}/
cp Public/WinSys.h ${binpath}/${includepath}/
cp Public/CircularBuffer.h ${binpath}/${includepath}/

##copy so 
cp build/linux/GridSeed ${binpath}/${libpath}/
cp Thridlibrary/linux/*.so.* ${binpath}/${libpath}/
echo create end......



