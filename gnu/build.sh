#!/usr/bin/bash

# build DLL
x86_64-w64-mingw32-g++ -c encoplot.cpp
x86_64-w64-mingw32-g++ -o encoplot.dll -shared encoplot.o -Wl,--output-def,encoplot.def
cp *.dll ../bin/x64/Debug

# build exe
x86_64-w64-mingw32-g++ -o encoplot -DEXE encoplot.cpp

# test exe
./encoplot.exe ../data/d1.txt ../data/d2.txt
