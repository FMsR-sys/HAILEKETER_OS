[bits 32]
;%macro IRQ_STUB 2
extern irq1_handler

global irq1_stub
irq1_stub:
    pusha
    call irq1_handler
    popa
    iret
;%endmacro
