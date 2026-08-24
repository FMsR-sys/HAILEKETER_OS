#!/bin/bash
set -e

SRC="./src"
OUT="./build"
CXX="i686-elf-g++"
NASM="nasm"

mkdir -p $OUT

$NASM -f elf32 $SRC/entry.asm -o $OUT/entry.o
$CXX -ffreestanding -m32 -c $SRC/kernel.cpp -o $OUT/kernel.o
$CXX -ffreestanding -m32 -T $SRC/linker.ld $OUT/entry.o $OUT/kernel.o -o $OUT/kernel.elf

