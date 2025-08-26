[bits 32]
global keyboard_entry
extern keyboard_handler

keyboard_entry:
    pusha                   ; Save all general-purpose registers
    push ds                 ; Save segment registers
    push es
    push fs
    push gs
    
    mov ax, 0x10            ; Load kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    call keyboard_handler   ; Call C handler
    
    pop gs                  ; Restore segment registers
    pop fs
    pop es
    pop ds
    popa                    ; Restore general-purpose registers
    iretd                   ; Return from interrupt
