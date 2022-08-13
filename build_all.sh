#!/bin/bash

if [ "$1" = 'r' ];
then
	py build.py -r && cd ./example/ && py build.py -r && cd ..
else
	py build.py && cd ./example/ && py build.py && cd ..
fi
