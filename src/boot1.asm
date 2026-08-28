[BITS 16]
[ORG 0x7C00]

start:
    mov [boot_drive], dl

    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x9000

    mov ax, 0xB800
    mov es, ax
    xor di, di

    mov si, msg
    xor di, di
    mov bl, 0x1F

print:
    lodsb
    or al, al
    jz read_boot2
    
    mov [es:di], al
    mov [es:di + 1], bl
    add di, 2
    jmp print

read_boot2:
    xor ax,ax
    mov es,ax

    mov ah, 02h
    mov al, 04h
    mov ch, 00h
    mov cl, 02h
    mov dh, 00h
    mov dl, [boot_drive]
    mov bx, 0x7E00
    
    int 13h
    jc disk_error

    jmp 0x0000:0x7E00 

disk_error:
    mov si, er_msg
    xor di, di
    mov bl, 0x4F
    jmp print

boot_drive db 0
msg db 'BIOS_1', 0
er_msg db 'READ ERR', 0 

times 510-($-$$) db 0
dw 0xAA55