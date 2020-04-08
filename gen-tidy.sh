#! /bin/bash

CURRENTDIR=$(pwd)

cd $CHIPSGEN
make clean
rm ./cmake_install.cmake
rm ./CMakeCache.txt
rm -r ./CMakeFiles
rm ./Makefile

cd $CURRENTDIR