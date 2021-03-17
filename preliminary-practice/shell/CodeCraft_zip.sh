#!/bin/bash

SCRIPT=$(readlink -f "$0")
BASEDIR=$(dirname "$SCRIPT")
cd $BASEDIR
cd ..

if [ ! -d CodeCraft-2021 ]
then
    echo "ERROR: $BASEDIR is not a valid directory of SDK_python for CodeCraft-2021."
    echo "  Please run this script in a regular directory of SDK_python."
    exit -1
fi

cd answer
mkdir tempDir
cp -r ../CodeCraft-2021 tempDir/
cp ../SDK_C++/*.sh tempDir/

cd tempDir
rm -rf ../CodeCraft-2021.zip
zip -r ../CodeCraft-2021.zip *
cd ..
rm -rf tempDir
