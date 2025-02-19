#!/bin/bash

if [ "$1" = 'r' ];
then
	python build.py -r && cd ./example/ && python build.py -r && cd ..
else
	python build.py && cd ./example/ && python build.py && cd ..
fi
