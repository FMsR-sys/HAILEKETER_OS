[bits 32]
section .text

extern isr_handler

global irq1
irq1:
    pusha
    push 0x21
    call isr_handler
    add esp, 4
    popa
    iret
