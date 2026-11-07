[bits 32]
extern irq1_handler
extern page_fault_handler

global irq1_stub
irq1_stub:
    pusha
    call irq1_handler
    popa
    iret

; 页错误 (INT 0x0E) 的 CPU 会额外压入一个 error code
; 栈布局（进入时）: [esp] = error_code, [esp+4] = EIP
global isr14_stub
isr14_stub:
    pusha
    mov eax, [esp + 32]      ; error_code (8 个通用寄存器 + 32 保留 = 32)

    push eax                 ; 参数: error code
    call page_fault_handler
    add esp, 4

    popa
    add esp, 4               ; 丢弃 error code
    iret
