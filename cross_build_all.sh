#!/bin/bash

if [ "$1" = 'r' ];
then
	python build.py -lw -r && python cross-compile.py -r && cd ./example/ && python build.py -lw -r && python cross-compile.py -r && cd ..
else
	python build.py -lw && python cross-compile.py && cd ./example/ && python build.py -lw && python cross-compile.py && cd ..
fi
