#define VGA_BYTES_LINE 160
#define VGA_TOTAL_BYTES 4000
#define VIDEO_MEMORY ((char*)0xb8000)
#define BUFFER_SIZE 128
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

static uint8_t key_buffer[BUFFER_SIZE];
static unsigned int head = 0;
static unsigned int cursor_x = 0;

static const char scan_code[128] = {
    0,  27, '1','2','3','4','5','6',
    '7','8','9','0','-','=','\b','\t',
    'q','w','e','r','t','y','u','i',
    'o','p','[',']','\n', 0, 'a','s',
    'd','f','g','h','j','k','l',';',
    '\'', '`', 0,'\\','z','x','c','v',
    'b','n','m',',','.','/', 0, '*',
    0, ' ', 0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0
};
static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

void yput_char(char c);
uint8_t inb(uint16_t port){
    
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
   
    return ret;
}

uint8_t keyboard_read_input(){
    int timeout = 1000000;  // arbitrary large number
while (!(inb(0x64) & 1)) {
    if (--timeout == 0) {
        // timeout happened, handle error or break
        return 0; // or special error code
    }
}
    
    uint8_t scancode = inb(0x60);
    char hello = scan_code[scancode];
    yput_char(hello);
    return inb(0x60);
}

void yprint(char message[]) {
    int i = 0;
    while(message[i]!='\0') {
        if (message[i] == '\n') {
            int currentLine = cursor_x / VGA_BYTES_LINE;
            cursor_x = (currentLine+1)*VGA_BYTES_LINE;
        }
        else if (cursor_x + 1 >= VGA_TOTAL_BYTES) {
            // Handle overflow by wrapping or stopping
            break;
        }
        else{
            VIDEO_MEMORY[cursor_x] = message[i];
            VIDEO_MEMORY[cursor_x + 1] = 0x0F;
            cursor_x += 2;
        }
        i++;
    }
}

void yput_char(char c){
    char buff[2] = {c,'\0'};
    yprint(buff);
}

void shell_command_ls_next(char shell_command[], int index){
    yprint("hello world");
}

void shell_command_ls(char shell_command[], int index) {
    // Check if it's actually "ls" command
    if(shell_command[0] == 'l' && shell_command[1] == 's' && (shell_command[2] == '\0' || shell_command[2] == ' ')) {
        shell_command_ls_next(shell_command, index);
        yput_char('\n');
    }
}

void Cbuffer(uint8_t scancode) {
    if (scancode & 0x80) {
    // This is a key RELEASE - ignore it
       return;
   }
    if(scancode == 0){
        yput_char('y');
    }
    
   if (scancode >= 128) return; // Ignore invalid scan codes
    
    char key = scan_code[scancode];
    
    if (key == '\n') {
        if (head > 0) {
            char shell_command[BUFFER_SIZE];
            int shell_counter = 0;
            // copy from key_buffer safely
            for (unsigned int i = 0; i < head && i < BUFFER_SIZE - 1; i++) {
                uint8_t code = key_buffer[i];
                if (code < 128 && scan_code[code] != 0)
                    shell_command[shell_counter++] = scan_code[code];
            }
            shell_command[shell_counter] = '\0';
            // clear buffer
            for (unsigned int i = 0; i < head; i++)
                key_buffer[i] = 0;
            head = 0;
            yput_char('\n');
            shell_command_ls(shell_command, 0);
        }
    } else if (key != 0) {
        
        // Only add to buffer if there's space and it's a valid key
        if (head < BUFFER_SIZE - 1) {
            key_buffer[head++] = scancode;
            yput_char(key);
        }
    }
}
void ps2_init() {
    while (inb(0x64) & 1) {
        inb(0x60);
    }
    outb(0x64, 0xAE); // Enable first PS/2 port

    outb(0x60, 0xFF); // Reset keyboard
    while (!(inb(0x64) & 1));
    uint8_t ack = inb(0x60); // Should be 0xFA (ACK)
    while (!(inb(0x64) & 1));
    uint8_t selftest = inb(0x60); // Should be 0xAA (self-test pass)

    outb(0x60, 0xF4); // Enable scanning
    while (!(inb(0x64) & 1));
    ack = inb(0x60);
    if (ack != 0xFA) {
        yprint("Keyboard init failed\n");
    }
}

void main() {
    char message[100] = " yaniOS Shell v0kk\nkk.";
    char message2[100] = "\n welcome\n to ";
    yprint(message2);
    yprint(message);
    yprint(message);
    yprint(message2);
    yprint(message);
    ps2_init();
     Cbuffer(0x1E);
     
    while(1){
      uint8_t scancode = keyboard_read_input();
        char key_char = scan_code[scancode];
    
    



Cbuffer(scancode);

   //asm volatile("hlt");
   }

}