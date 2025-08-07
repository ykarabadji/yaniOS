. = 0x7e00;

SECTIONS {
  .text : { *(.text) }
  .rodata : { *(.rodata*) }
  .data : { *(.data) }
  .bss : {
    *(.bss)
    *(COMMON)
  }
}
