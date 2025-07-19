#!/bin/bash

if [ "$1" = 'r' ];
then
	python build.py -a8 -r
	python cross-compile-armv8.py -r

	python build.py -lw -r
	python cross-compile-win.py -r

	cd ./example/
	python build.py -a8 -r
	python cross-compile-armv8.py -r

	python build.py -lw -r
	python cross-compile-win.py -r
	cd ..
else
	python build.py -a8
	python cross-compile-armv8.py

	python build.py -lw
	python cross-compile-win.py

	cd ./example/
	python build.py -a8
	python cross-compile-armv8.py

	python build.py -lw
	python cross-compile-win.py
	cd ..
fi
