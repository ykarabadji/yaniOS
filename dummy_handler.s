[bits 32]
; dummy_handler.s
global dummy_handler,divide_error_handler, double_fault_handler, gp_fault_handler, page_fault_handler

divide_error_handler:
    cli
    mov byte [0xB8000], '0'  ; Print '0' for divide error
    hlt

double_fault_handler:
    cli
    mov byte [0xB8000], '8'  ; Print '8' for double fault
    hlt

gp_fault_handler:
    cli
    mov byte [0xB8000], 'D'  ; Print 'D' for general protection
    hlt

page_fault_handler:
    cli
    mov byte [0xB8000], 'E'  ; Print 'E' for page fault
    hlt
dummy_handler:
    pusha
    iret