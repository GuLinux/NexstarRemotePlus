#!/bin/bash
WDIR="$( cd "$( dirname "$0" )"; pwd )"
cd "$WDIR"
echo '#pragma once'cat >icons.h
for file in *.png; do
    echo "Generating icon for $file" >&2
    ./image2arduino.py -r 32 32 -o $( basename "$file" .png ) -p icon_ -b '#ffffff' -i $file >> icons.h
done
