#!/bin/bash

for FILE in *.txt
do
   ../ciw.exe $FILE > ./temp/`basename $FILE .txt`.asm
done

diff Regress temp
