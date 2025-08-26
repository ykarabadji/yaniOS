#ifndef IDT_H
#define IDT_H
#include "print.h"
extern void keyboard_entry(); // From the ASM file

// DEFINITIONS
#define IDT_ENTRIES 256
#define IDT_BASE    0x10000
// Replace the problematic sizeof() with explicit calculation
#define IDT_ENTRY_SIZE 8  // Each IDT entry is exactly 8 bytes
#define IDT_LIMIT   (IDT_ENTRIES * IDT_ENTRY_SIZE - 1)
// STRUCT DEFINITIONS

// Must be packed to prevent padding
struct InterruptDes32 {
    uint16_t offset_1;        // offset bits 0..15
    uint16_t selector;        // a code segment selector in GDT or LDT
    uint8_t  zero;            // unused, set to 0
    uint8_t  type_attributes; // gate type, dpl, and p fields
    uint16_t offset_2;        // offset bits 16..31
} __attribute__((packed));     // <- THIS IS REQUIRED
volatile struct InterruptDes32 IDT[IDT_ENTRIES] __attribute__((section(".idt")));
// IDTR structure
struct IDTR {
    uint16_t limit;           // LIMIT comes first (NOT base!)
    uint32_t base;
} __attribute__((packed));
static struct IDTR idtr;

// IDT DECLARATION

// You can place this at a specific address if you're managing memory manually


// Set up the IDTR
void init_idt(){
    idtr.limit = IDT_LIMIT;
    idtr.base = (uint32_t)IDT;
}

void backspace_handler() {
    if (cursor_x == 0) {
        // At beginning of screen, can't go back further
        return;
    }
    
    if (cursor_x % VGA_BYTES_LINE == 0) {
        // At beginning of a line (but not screen beginning)
        // Move to end of previous line
        cursor_x -= 2; // Move to the last character of previous line
    } else {
        // Normal case - just move back one character
        cursor_x -= 2;
    }
    
    // Clear the character
    VIDEO_MEMORY[cursor_x] = ' ';
    VIDEO_MEMORY[cursor_x + 1] = 0x00; // Keep your color scheme
    
    
}

// Load IDT
void load_idt() {
    yput_char('c');
    asm volatile ("lidt %0" : : "m"(idtr));
        yput_char('D');  // IDT load completed
}
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

void pic_remap() {
    yput_char('k');
    // Initialization Control Word (ICW)
    outb(0x20, 0x11); // Start init for master
    outb(0xA0, 0x11); // Start init for slave

    outb(0x21, 0x20); // Remap master IRQs to 0x20–0x27
    outb(0xA1, 0x28); // Remap slave IRQs to 0x28–0x2F

    outb(0x21, 0x04); // Tell master PIC that slave is at IRQ2 (0000 0100)
    outb(0xA1, 0x02); // Tell slave PIC its cascade identity (0000 0010)

    outb(0x21, 0x01); // Set x86 mode
    outb(0xA1, 0x01);

 // Unmask only IRQ1 (keyboard) on master PIC
// IRQ1 corresponds to bit 1 (value 0xFD = 11111101)
outb(0x21, 0xFD);  // Enable only IRQ1
outb(0xA1, 0xFF);  // Mask all IRQs on slave
 yput_char('Q');  // PIC remap completed
}



void keyboard_handler() {
    uint8_t scancode = inb(0x60);  // Read key scancode from keyboard
    char c = 0;        
                // Default to 0 (no character)
                
   
    switch (scancode) {
        case 0x02: c = '1'; break;
        case 0x03: c = '2'; break;
        case 0x04: c = '3'; break;
        case 0x05: c = '4'; break;
        case 0x06: c = '5'; break;
        case 0x07: c = '6'; break;
        case 0x08: c = '7'; break;
        case 0x09: c = '8'; break;
        case 0x0A: c = '9'; break;
        case 0x0B: c = '0'; break;
        case 0x0C: c = '-'; break;
        case 0x0D: c = '='; break;
        case 0x0E: c = '\b';
           backspace_handler();
           outb(0x20, 0x20);
            return;
         break;  // Backspace
        case 0x0F: c = '\t'; break;  // Tab
        case 0x10: c = 'q'; break;
        case 0x11: c = 'w'; break;
        case 0x12: c = 'e'; break;
        case 0x13: c = 'r'; break;
        case 0x14: c = 't'; break;
        case 0x15: c = 'y'; break;
        case 0x16: c = 'u'; break;
        case 0x17: c = 'i'; break;
        case 0x18: c = 'o'; break;
        case 0x19: c = 'p'; break;
        case 0x1A: c = '['; break;
        case 0x1B: c = ']'; break;
        case 0x1C: c = '\n'; break;  // Enter
        case 0x1E: c = 'a'; break;
        case 0x1F: c = 's'; break;
        case 0x20: c = 'd'; break;
        case 0x21: c = 'f'; break;
        case 0x22: c = 'g'; break;
        case 0x23: c = 'h'; break;
        case 0x24: c = 'j'; break;
        case 0x25: c = 'k'; break;
        case 0x26: c = 'l'; break;
        case 0x27: c = ';'; break;
        case 0x28: c = '\''; break;
        case 0x29: c = '`'; break;
        case 0x2C: c = 'z'; break;
        case 0x2D: c = 'x'; break;
        case 0x2E: c = 'c'; break;
        case 0x2F: c = 'v'; break;
        case 0x30: c = 'b'; break;
        case 0x31: c = 'n'; break;
        case 0x32: c = 'm'; break;
        case 0x33: c = ','; break;
        case 0x34: c = '.'; break;
        case 0x35: c = '/'; break;
        case 0x39: c = ' '; break;   // Space
        default:
            // Unhandled key or key release (>= 0x80)
            break;
    }
    
    if (c) {
        yput_char(c);  // Assuming this function prints a char to the screen
    }

    // Send End of Interrupt (EOI) to PIC
    outb(0x20, 0x20);
    
}
void idt_set_gate(int n, uint32_t handler, uint16_t selector, uint8_t flags) {
    
    IDT[n].offset_1 = handler & 0xFFFF;
    IDT[n].selector = selector;
    IDT[n].zero = 0;
    IDT[n].type_attributes = flags;
    IDT[n].offset_2 = (handler >> 16) & 0xFFFF;
}

// Example: set IRQ1 = vector 0x21

#endif