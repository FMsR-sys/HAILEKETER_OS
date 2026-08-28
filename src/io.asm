[bits 32]
section .text

global io_inb
global io_outb

io_inb:
    mov  dx, [esp + 4]
    in   al, dx
    ret

io_outb:
    mov  dx, [esp + 4]
    mov  al, [esp + 8]
    out  dx, al
    ret
