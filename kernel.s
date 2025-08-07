; kernel.s - The kernel's entry point in 32-bit protected mode
[bits 32]

; Export the _start label so the linker can find it
[global _start]

; Inform the linker that the 'main' function is defined elsewhere (in main.c)
[extern main]

_start:
    ; A proper kernel entry point should set up all segment registers.
    ; This ensures a consistent environment for C code.
    
    ; Load the data segment selector into all other segment registers.
    ; This is necessary for a flat memory model.
    mov ax, 0x10          ; 0x10 is the data segment selector from your GDT
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax            ; Set the stack segment
      mov     byte [ds:0xB8005], 0x07
      mov     byte [ds:0xB8004], 'P'
    ; The 'switch32bit' routine already set a stack, but it's good practice
    ; to confirm and set it again for the kernel's use.
    ; Set the stack pointer (ESP) to the top of the stack
    mov esp, 0x90000      ; Set ESP to the top of our 576KB stack (below 1MB)
    
    ; Call the main C function. The call instruction will push the return
    ; address onto the stack, so the stack must be properly set up.
    call main
    jmp $

    ; If main() returns, halt the CPU to prevent it from executing garbage
.halt:
    hlt                   ; Halt the CPU
    jmp .halt             ; Loop forever just in case the hlt instruction is ignored
