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
#define LEFT_SHIFT    0x2A
#define RIGHT_SHIFT   0x36
#define CAPS_LOCK     0x3A
#define LEFT_CTRL     0x1D
#define LEFT_ALT      0x38
#define BUFFER_COMMAND_SIZE 256
// Define scancode constants for clarity
#define LEFT_SHIFT_PRESS    0x2A
#define LEFT_SHIFT_RELEASE  0xAA
#define RIGHT_SHIFT_PRESS   0x36
#define RIGHT_SHIFT_RELEASE 0xB6
#define CAPS_LOCK_RELEASE     0xBA
static uint8_t shift_pressed = 0;
static uint8_t caps_lock = 0;
static char COMMAND_BUFFER[BUFFER_COMMAND_SIZE];
static int command_tracker = 0;
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
void Init_command_buffer(){
    for (int i=0;i<BUFFER_COMMAND_SIZE;i++){
        COMMAND_BUFFER[i] = '\0';

    }
}

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
    uint8_t scancode = inb(0x60);
    
    // Handle shift key state changes
    if(scancode == CAPS_LOCK){
        if(caps_lock == 1){
            caps_lock = 0;
            goto eoi;
        }else{
            caps_lock = 1;
            goto eoi;
        }
    }
    if(scancode == LEFT_SHIFT_PRESS || scancode == RIGHT_SHIFT_PRESS) {
        shift_pressed = 1;
        goto eoi;

    }
    if(scancode == LEFT_SHIFT_RELEASE || scancode == RIGHT_SHIFT_RELEASE) {
        shift_pressed = 0;
        goto eoi;
    }

    // Ignore key releases (except shifts handled above)
    if(scancode & 0x80) goto eoi;

    // Handle key presses
    char c = 0;
    switch(scancode) {
    case 0x02: c = shift_pressed ? '!' : '1';
               c = caps_lock ? '!' : '1';
               break;
    case 0x03: c = shift_pressed ? '@' : '2';
               c = caps_lock ? '@' : '2';
               break;
    case 0x04: c = shift_pressed ? '#' : '3';
               c = caps_lock ? '#' : '3';
               break;
    case 0x05: c = shift_pressed ? '$' : '4';
               c = caps_lock ? '$' : '4';
               break;
    case 0x06: c = shift_pressed ? '%' : '5';
               c = caps_lock ? '%' : '5';
               break;
    case 0x07: c = shift_pressed ? '^' : '6';
               c = caps_lock ? '^' : '6';
               break;
    case 0x08: c = shift_pressed ? '&' : '7';
               c = caps_lock ? '&' : '7';
               break;
    case 0x09: c = shift_pressed ? '*' : '8';
               c = caps_lock ? '*' : '8';
               break;
    case 0x0A: c = shift_pressed ? '(' : '9';
               c = caps_lock ? '(' : '9';
               break;
    case 0x0B: c = shift_pressed ? ')' : '0';
               c = caps_lock ? ')' : '0';
               break;
    case 0x0C: c = shift_pressed ? '_' : '-';
               c = caps_lock ? '_' : '-';
               break;
    case 0x0D: c = shift_pressed ? '+' : '=';
               c = caps_lock ? '+' : '=';
               break;
    case 0x0E: 
        backspace_handler();
        goto eoi;
    case 0x0F: c = '\t';
               c = caps_lock ? '\t' : '\t';
               break;
    case 0x10: c = shift_pressed ? 'Q' : 'q';
               c = caps_lock ? 'Q' : 'q';
               break;
    case 0x11: c = shift_pressed ? 'W' : 'w';
               c = caps_lock ? 'W' : 'w';
               break;
    case 0x12: c = shift_pressed ? 'E' : 'e';
               c = caps_lock ? 'E' : 'e';
               break;
    case 0x13: c = shift_pressed ? 'R' : 'r';
               c = caps_lock ? 'R' : 'r';
               break;
    case 0x14: c = shift_pressed ? 'T' : 't';
               c = caps_lock ? 'T' : 't';
               break;
    case 0x15: c = shift_pressed ? 'Y' : 'y';
               c = caps_lock ? 'Y' : 'y';
               break;
    case 0x16: c = shift_pressed ? 'U' : 'u';
               c = caps_lock ? 'U' : 'u';
               break;
    case 0x17: c = shift_pressed ? 'I' : 'i';
               c = caps_lock ? 'I' : 'i';
               break;
    case 0x18: c = shift_pressed ? 'O' : 'o';
               c = caps_lock ? 'O' : 'o';
               break;
    case 0x19: c = shift_pressed ? 'P' : 'p';
               c = caps_lock ? 'P' : 'p';
               break;
    case 0x1A: c = shift_pressed ? '{' : '[';
               c = caps_lock ? '{' : '[';
               break;
    case 0x1B: c = shift_pressed ? '}' : ']';
               c = caps_lock ? '}' : ']';
               break;
    case 0x1C: c = '\n';
               c = caps_lock ? '\n' : '\n';
               break;
    case 0x1E: c = shift_pressed ? 'A' : 'a';
               c = caps_lock ? 'A' : 'a';
               break;
    case 0x1F: c = shift_pressed ? 'S' : 's';
               c = caps_lock ? 'S' : 's';
               break;
    case 0x20: c = shift_pressed ? 'D' : 'd';
               c = caps_lock ? 'D' : 'd';
               break;
    case 0x21: c = shift_pressed ? 'F' : 'f';
               c = caps_lock ? 'F' : 'f';
               break;
    case 0x22: c = shift_pressed ? 'G' : 'g';
               c = caps_lock ? 'G' : 'g';
               break;
    case 0x23: c = shift_pressed ? 'H' : 'h';
               c = caps_lock ? 'H' : 'h';
               break;
    case 0x24: c = shift_pressed ? 'J' : 'j';
               c = caps_lock ? 'J' : 'j';
               break;
    case 0x25: c = shift_pressed ? 'K' : 'k';
               c = caps_lock ? 'K' : 'k';
               break;
    case 0x26: c = shift_pressed ? 'L' : 'l';
               c = caps_lock ? 'L' : 'l';
               break;
    case 0x27: c = shift_pressed ? ':' : ';';
               c = caps_lock ? ':' : ';';
               break;
    case 0x28: c = shift_pressed ? '"' : '\'';
               c = caps_lock ? '"' : '\'';
               break;
    case 0x29: c = shift_pressed ? '~' : '`';
               c = caps_lock ? '~' : '`';
               break;
    case 0x2C: c = shift_pressed ? 'Z' : 'z';
               c = caps_lock ? 'Z' : 'z';
               break;
    case 0x2D: c = shift_pressed ? 'X' : 'x';
               c = caps_lock ? 'X' : 'x';
               break;
    case 0x2E: c = shift_pressed ? 'C' : 'c';
               c = caps_lock ? 'C' : 'c';
               break;
    case 0x2F: c = shift_pressed ? 'V' : 'v';
               c = caps_lock ? 'V' : 'v';
               break;
    case 0x30: c = shift_pressed ? 'B' : 'b';
               c = caps_lock ? 'B' : 'b';
               break;
    case 0x31: c = shift_pressed ? 'N' : 'n';
               c = caps_lock ? 'N' : 'n';
               break;
    case 0x32: c = shift_pressed ? 'M' : 'm';
               c = caps_lock ? 'M' : 'm';
               break;
    case 0x33: c = shift_pressed ? '<' : ',';
               c = caps_lock ? '<' : ',';
               break;
    case 0x34: c = shift_pressed ? '>' : '.';
               c = caps_lock ? '>' : '.';
               break;
    case 0x35: c = shift_pressed ? '?' : '/';
               c = caps_lock ? '?' : '/';
               break;
    case 0x39: c = ' ';
               c = caps_lock ? ' ' : ' ';
               break;
    default: goto eoi;
}
    if(c) {
        yput_char(c);
        if(c == '\n'){
            shell_command_ls(COMMAND_BUFFER);
            Init_command_buffer();
            command_tracker  = 0;
            
        }
         else if (command_tracker < BUFFER_COMMAND_SIZE - 1) {
            // Only add if there's space (leave room for null terminator)
            COMMAND_BUFFER[command_tracker] = c;
            command_tracker++;
        }
    }

eoi:
    outb(0x20, 0x20); // Send EOI
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