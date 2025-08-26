; bootsect.asm
org 0x7c00            ; BIOS loads boot sector here

start:
    mov ah, 0x0f
    int 0x10          ; BIOS call to get video mode

    jmp $             ; infinite loop

times 510-($-$$) db 0 ; fill to 510 bytes
dw 0xAA55             ; boot signature

