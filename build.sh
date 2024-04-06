#!/bin/bash

CMAKE_OPTIONS="-DCMAKE_BUILD_TYPE=Debug"

mkdir build

echo -e "\e[37m\n\n---\n Building for Linux\n---\n\e[0m"
cmake . -B build/linux "$CMAKE_OPTIONS" && make -C build/linux

#echo -e "\e[37m\n\n---\n Building for Windows\n---\n\e[0m"
#WINDOWS_BUILD=1 mingw64-cmake . -B build/win "$CMAKE_OPTIONS" && mingw64-make -C build/win

mkdir build/binaries
rm build/binaries/*

cp build/linux/main build/binaries
cp build/win/main.exe build/binaries
