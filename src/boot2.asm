[bits 16]
org 0x7e00

E820_BUF        equ 0x5000
E820_ENTRY_CNT  equ 0x500
KERNEL_BASE     equ 0x100000
KERNEL_SIZE     equ 32 * 512
KERNEL_END      equ KERNEL_BASE + KERNEL_SIZE
STACK_SAFE_LOW  equ 0x80000
STACK_SAFE_HIGH equ 0xA0000

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
    mov al,32
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

    xor ax, ax
    mov [E820_ENTRY_CNT], ax

    mov ebx, 0

    mov ax, ds
    mov es, ax
    mov di, E820_BUF

.e820_loop:
    mov eax, 0xE820
    mov edx, 0x534D4150
    mov ecx, 20

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
    call get_e820_map
    call test_e820_count


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
    xor di,di
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
test_e820_count:
    mov ax, 0xB800
    mov es, ax
    mov di, 0

    mov cx, [E820_ENTRY_CNT]

    mov al, 'C'
    mov ah, 0x1E
    mov [es:di], ax
    add di, 2

    mov al, '='
    mov [es:di], ax
    add di, 2

    mov al, cl
    add al, '0'
    mov [es:di], ax

    ret


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

    call zero_usable_ext_mem

    mov edi, 0xA0000
    xor eax, eax
    mov ecx, 2048
    rep stosd

.flush:
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
    jmp 0x08:KERNEL_BASE

zero_usable_ext_mem:
    push ebx
    push esi
    push edi
    push ecx
    push edx
    push ebp

    mov esi, E820_BUF
    xor ebx, ebx
    mov bx, [E820_ENTRY_CNT]
.entry_loop:
    cmp ebx, 0
    je .zero_finish
    dec ebx

    mov edx, [esi + 16]
    cmp edx, 1
    jne .next_entry

    mov eax, [esi + 0]
    mov edx, [esi + 4]
    mov ecx, [esi + 8]
    mov ebp, [esi + 12]

    cmp edx, 0
    jne .next_entry

    cmp eax, KERNEL_BASE
    jb .next_entry

    mov edx,eax
    add edx,ecx

    cmp eax, KERNEL_END
    jae .check_stack
    mov eax,KERNEL_END
    cmp edx,eax
    jbe .next_entry

.check_stack:
    cmp eax, STACK_SAFE_HIGH
    jb .next_entry

.do_zero:
    push ecx
    push eax
    mov edi,eax
    xor eax,eax
    rep stosb
    pop eax
    pop ecx

.next_entry:
    add esi, 20
    jmp .entry_loop

.zero_finish:
    pop ebp
    pop edx
    pop ecx
    pop edi
    pop esi
    pop ebx
    ret

hang:
    hlt
    jmp hang

er_msg db 'BOOT2 READ ERR',0
boot_ok_str db 'Bootloader finished, jumping kernel...',0

times 2046 - ($ - $$) db 0
dw 0xAA55
