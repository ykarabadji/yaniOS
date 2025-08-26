OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)     /* Kernel entry point symbol, define in your C/assembly */

SECTIONS
{
    /* Place code (text) at 0x7e00 */
    . = 0x7e00;
    .text : {
        *(.text)           /* All code */
    }

    /* Place .rodata into .data so literals are in RAM */
    .data : {
        *(.data)           /* Initialized data */
        *(.rodata*)        /* String literals go here too */
    }

    /* Uninitialized data (BSS) */
    .bss : {
        *(.bss)
        *(COMMON)
    }
    /* Place IDT at 0x9000 */
    .idt 0x10000 : {
        *(.idt)
    }
}
