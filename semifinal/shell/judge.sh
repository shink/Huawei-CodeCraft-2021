#!/bin/bash

SCRIPT=$(readlink -f "$0")
BASEDIR=$(dirname "$SCRIPT")
SCRIPT_LOCATION="/home/shenke/Develop/2021HWAutoGrader/"

cd $BASEDIR
sh build.sh

cd $SCRIPT_LOCATION
python3 autograder.py
