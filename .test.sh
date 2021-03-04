#!/bin/bash

set -e

if [ ! -d examples ]; then
	make all -f makefile.gnucc
	exit 0
fi

for file in examples/*.c_
do
	rm -f src/main.*
	echo $file
	cp $file src/main.c
	make all -f makefile.gnucc
done

for file in examples/*.cpp_
do
	rm -f src/main.*
	echo $file
	cp $file src/main.cpp
	make all -f makefile.gnucc
done
