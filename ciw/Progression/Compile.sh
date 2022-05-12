#!/bin/bash

../ciw.exe $1.txt > $1.asm
./asm.exe $1.asm $1.rom
