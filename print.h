
#ifndef PRINT_H
#define PRINT_H

#define VGA_BYTES_LINE 160
#define VGA_TOTAL_BYTES 4000
#define VIDEO_MEMORY ((char*)0xb8000)
#define BUFFER_SIZE 128
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;


static unsigned int cursor_x = 0;




void yput_char(char c);
void yprint(char message[]);




void yprint(char message[]) {
    int i = 0;
    while(message[i]!='\0') {
        if (message[i] == '\n') {
            int currentLine = cursor_x / VGA_BYTES_LINE;
            cursor_x = (currentLine+1)*VGA_BYTES_LINE;
            yput_char('>');
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

void shell_command_ls_next(char shell_command[]){
    yprint("hello world");
    
}

void shell_command_ls(char shell_command[]) {
    // Check if it's actually "ls" command
    if(shell_command[0] == 'l' && shell_command[1] == 's' && (shell_command[2] == '\0' || shell_command[2] == ' ')) {
        shell_command_ls_next(shell_command);
        yput_char('\n');
    }
}
#endif // PRINT_H