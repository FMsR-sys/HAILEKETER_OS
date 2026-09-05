#!/bin/bash
set -e

rm -f *.o kernel.elf kernel.bin
nasm -f elf32 paging_asm.asm -o paging_asm.o
nasm -f elf32 idt.asm -o idt_asm.o
nasm -f bin boot1.asm -o boot1.bin
nasm -f bin boot2.asm -o boot2.bin
cat boot1.bin boot2.bin > boot.bin
/c/i686-elf-tools-windows/bin/i686-elf-g++ -ffreestanding -m32 -c kernel.cpp -o kernel.o
/c/i686-elf-tools-windows/bin/i686-elf-g++ -ffreestanding -m32 -c vga.cpp -o vga.o
/c/i686-elf-tools-windows/bin/i686-elf-g++ -ffreestanding -m32 -c io.cpp -o io.o
/c/i686-elf-tools-windows/bin/i686-elf-g++ -ffreestanding -m32 -c e820.cpp -o e820.o
/c/i686-elf-tools-windows/bin/i686-elf-g++ -ffreestanding -m32 -c paging.cpp -o paging.o
/c/i686-elf-tools-windows/bin/i686-elf-g++ -ffreestanding -m32 -c pmm.cpp -o pmm.o
/c/i686-elf-tools-windows/bin/i686-elf-g++ -ffreestanding -m32 -c pic.cpp -o pic.o
/c/i686-elf-tools-windows/bin/i686-elf-g++ -ffreestanding -m32 -c isr.cpp -o isr.o
/c/i686-elf-tools-windows/bin/i686-elf-g++ -ffreestanding -m32 -c idt.cpp -o idt.o
/c/i686-elf-tools-windows/bin/i686-elf-ld -T linker.ld kernel.o vga.o io.o paging_asm.o e820.o paging.o pmm.o pic.o idt_asm.o isr.o idt.o -o kernel.elf
/c/i686-elf-tools-windows/bin/i686-elf-objcopy -O binary kernel.elf kernel.bin
dd if=boot.bin of=boot.img bs=512 conv=notrunc
dd if=kernel.bin of=boot.img bs=512 seek=5 conv=notrunc
qemu-system-i386 -m 512M -drive format=raw,file=boot.img,if=floppy