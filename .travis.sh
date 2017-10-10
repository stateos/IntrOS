#!/bin/bash

set -e

rm -f src/main.c
rm -f src/main.cpp

for file in examples/*.c\ *
do
cp "$file" src/main.c
touch src/main.c
make all GNUCC=arm-none-eabi- -f makefile.gnucc
rm -f src/main.c
done

for file in examples/*.cpp*
do
cp "$file" src/main.cpp
touch src/main.cpp
make all GNUCC=arm-none-eabi- -f makefile.gnucc
rm -f src/main.cpp
done
