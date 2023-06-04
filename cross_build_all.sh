#!/bin/bash

if [ "$1" = 'r' ];
then
	py build.py -lw -r && py cross-compile.py -r && cd ./example/ && py build.py -lw -r && py cross-compile.py -r && cd ..
else
	py build.py -lw && py cross-compile.py && cd ./example/ && py build.py -lw && py cross-compile.py && cd ..
fi
