#!/bin/bash
set -e
nasm -f elf32 io.asm -o io_asm.o
nasm -f elf32 isr.asm -o isr_asm.o
nasm -f elf32 paging_asm.asm -o paging_asm.o
nasm -f bin boot1.asm -o boot1.bin
nasm -f bin boot2.asm -o boot2.bin
cat boot1.bin boot2.bin > boot.bin
i686-elf-g++ -ffreestanding -m32 -c kernel.cpp -o kernel.o
i686-elf-g++ -ffreestanding -m32 -c vga.cpp -o vga.o
i686-elf-g++ -ffreestanding -m32 -c pic.cpp -o pic.o
i686-elf-g++ -ffreestanding -m32 -c io.cpp -o io.o
i686-elf-g++ -ffreestanding -m32 -c isr.cpp -o isr.o
i686-elf-g++ -ffreestanding -m32 -c e820.cpp -o e820.o
i686-elf-g++ -ffreestanding -m32 -c paging.cpp -o paging.o
i686-elf-ld -T linker.ld kernel.o vga.o io.o io_asm.o pic.o isr.o isr_asm.o paging_asm.o e820.o paging.o -o kernel.elf
i686-elf-objcopy -O binary kernel.elf kernel.bin
dd if=boot.bin of=boot.img bs=512 conv=notrunc
dd if=kernel.bin of=boot.img bs=512 seek=5 conv=notrunc
qemu-system-i386 -drive format=raw,file=boot.img,if=floppy