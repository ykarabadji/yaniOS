[bits 16]
[org 0x7c00]

start:
    
     
    
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov bx, ax
    mov [diskNum], dl       ; Save boot disk number
    ; In boot.s somewhere before the call:
      ;clear screen and configure gpu
      mov ah, 0x0
      mov al, 0x2
      int 0x10
      xor ax,ax

    
    call read_kernel
    
    call switch32bit       ; defined in switch32bit.s

    jmp $

read_kernel:
    pusha
    mov ah, 0x02            ; BIOS read sectors
    mov al, 50              ; Load 69 sectors (35,328 bytes)
    mov ch, 0               ; Cylinder
    mov cl, 2               ; Sector number (1-based)
    mov dh, 0               ; Head
    mov dl, [diskNum]       ; Disk number
    mov bx, 0x7e00          ; Destination offset
   
    
    int 0x13
    jc disk_error
    popa
    ret

disk_error:
    mov si, msg_error
    call print_error
print_error:
    lodsb
    or al, al
    jz .done
    mov ah, 0x0E
    int 0x10
    jmp print_error
.done:
    hlt



diskNum db 0
msg_error db 'Disk read error!', 0
%include "switch32bit.s"
times 510 - ($ - $$) db 0
dw 0xAA55


