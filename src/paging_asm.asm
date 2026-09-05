[bits 32]
global enable_paging
enable_paging:
    mov cr3, eax
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    jmp 0x08:flush_pipeline
flush_pipeline:
    ret
