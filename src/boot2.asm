[bits 16]
org 0x7e00
E820_BUF        equ 0x5000
E820_ENTRY_CNT  equ 0x500
KERNEL_BASE     equ 0x100000
KERNEL_SIZE     equ 8 * 512
KERNEL_END      equ KERNEL_BASE + KERNEL_SIZE
start:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x9000
    mov ax,0xB800
    mov es,ax
    xor di,di
    mov cx,2000
    mov ax,0x1E20
    rep stosw
    call get_e820_map
    xor ax,ax
    mov es,ax
    mov ah,0x02
    mov al,8
    mov ch,0
    mov cl,6
    mov dh,0
    mov bx,0x1000
    int 0x13
    jc read_error
    call enable_a20
    cli
    lgdt [gdt_ptr]
    mov eax,cr0
    or eax,1
    mov cr0,eax
    jmp 0x08:code32

get_e820_map:
    push bx
    push di
    push cx
    push dx
    push si
    xor di, di
    mov [E820_ENTRY_CNT], di
    mov ebx, 0
.e820_loop:
    mov eax, 0xE820
    mov edx, 0x534D4150
    mov ecx, 20
    mov di, E820_BUF
    int 0x15
    jc .e820_done
    cmp eax, 0x534D4150
    jne .e820_done
    inc word [E820_ENTRY_CNT]
    add di, 20
    cmp ebx, 0
    jnz .e820_loop
.e820_done:
    pop si
    pop dx
    pop cx
    pop di
    pop bx
    ret

enable_a20:
    push ax
    in al, 0x64
    test al, 2
    jnz $-4
    mov al, 0xD1
    out 0x64, al
    in al, 0x64
    test al, 2
    jnz $-4
    mov al, 0xDF
    out 0x60, al
    pop ax
    ret

read_error:
    mov ax,0xB800
    mov es,ax
    mov si, er_msg
    mov bl,0x4F
print_err:
    lodsb
    or al,al
    jz halt
    mov [es:di],al
    mov [es:di+1],bl
    add di,2
    jmp print_err
halt:
    cli
.h: hlt
    jmp .h

gdt_start:
    dq 0
gdt_code:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0b10011010
    db 0b11001111
    db 0x00
gdt_data:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0b10010010
    db 0b11001111
    db 0x00
gdt_end:
gdt_ptr:
    dw gdt_end - gdt_start -1
    dd gdt_start

[bits 32]
code32:
    mov ax,0x10
    mov ds,ax
    mov es,ax
    mov fs,ax
    mov gs,ax
    mov ss,ax
    mov esp,0x90000

    cld
    mov esi, 0x1000
    mov edi, KERNEL_BASE
    mov ecx, KERNEL_SIZE
    rep movsb

    ; ===== 临时关闭危险的内存清零函数 =====
    ;call zero_usable_ext_mem

    ; 打印提示文字【开启分页之前！】
    mov esi, boot_ok_str
    mov edi, 0xB8000 + 160
    mov ah, 0x1E
.print_ok:
    lodsb
    test al,al
    jz .print_done
    mov [edi], al
    mov [edi+1], ah
    add edi,2
    jmp .print_ok
.print_done:

    ; 分页初始化 0‑4MB 恒等映射
    mov edi, 0x200000
    xor eax,eax
    mov ecx, 1024
    rep stosd

    mov edi, 0x201000
    mov ebx, 0
    mov ecx, 1024
.set_pt:
    mov eax, ebx
    or eax, 3
    mov dword [edi], eax
    add ebx, 0x1000
    add edi, 4
    loop .set_pt

    mov dword [0x200000], 0x201003

    mov eax, 0x200000
    mov cr3, eax
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax
    jmp .flush
.flush:

    ; 跳转到内核
    jmp 0x08:KERNEL_BASE

hang:
    hlt
    jmp hang

er_msg db 'BOOT2 READ ERR',0
boot_ok_str db 'Bootloader finished, jumping kernel...',0

times 2046 - ($ - $$) db 0
dw 0xAA55
