#include "idt.h"
#include "print.h"
extern void keyboard_entry();
extern void dummy_handler();
extern void divide_error_handler();
extern void double_fault_handler();
extern void gp_fault_handler();
extern void page_fault_handler();

void main() {
    char message[100] = "yaniOS Shell v0kk\nkk.";
    yprint(message);
    yprint(message);
   
    // First, initialize IDT and set up all entries
    init_idt();
    
    yprint(message);
    
    // SET UP CRITICAL EXCEPTION HANDLERS FIRST
    idt_set_gate(0x00, (uint32_t)divide_error_handler, 0x08, 0x8E);  // Divide error
    idt_set_gate(0x08, (uint32_t)double_fault_handler, 0x08, 0x8E);   // Double fault
    idt_set_gate(0x0D, (uint32_t)gp_fault_handler, 0x08, 0x8E);       // General protection
    idt_set_gate(0x0E, (uint32_t)page_fault_handler, 0x08, 0x8E);     // Page fault
    
    // Then set up keyboard interrupt
    idt_set_gate(0x21, (uint32_t)keyboard_entry, 0x08, 0x8E);
    
    yprint("Before PIC remap\n");  // This should print
    
    // Then remap PIC
    pic_remap();  
    
    yprint("Before IDT load\n");  // Check if this prints
    
    // Then load IDT and enable interrupts
    load_idt();   
    
    yprint("Before STI\n");  // Check if this prints
    
    asm volatile("sti");    
    
    yprint("After STI - ready for input\n");  // This might not print if exceptions occur
    
    yput_char('q'); 
    
    while(1) {
        asm volatile("hlt");
    }
}