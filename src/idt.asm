[bits 32]

extern irq1_handler

global irq1_stub
irq1_stub:
    pusha
    call irq1_handler
    popa
    iret
