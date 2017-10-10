#!/bin/bash

set -e

if [ ! -d examples ]; then
	make all GNUCC=arm-none-eabi- -f makefile.gnucc
	exit 0
fi

rm -f src/main.cpp
for file in examples/*.c\ *
do
	echo $file
	cp "$file" src/main.c
	touch src/main.c
	make all GNUCC=arm-none-eabi- -f makefile.gnucc
done

rm -f src/main.c
for file in examples/*.cpp*
do
	echo $file
	cp "$file" src/main.cpp
	touch src/main.cpp
	make all GNUCC=arm-none-eabi- -f makefile.gnucc
done
