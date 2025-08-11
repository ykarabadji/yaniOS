#define VGA_BYTES_LINE 160
#define VGA_TOTAL_BYTES 4000
typedef unsigned int  uint32_t;
typedef unsigned char uint8_t;

#define BUFFER_SIZE 128
static uint8_t key_buffer[BUFFER_SIZE];
static unsigned int head = 0;

/*void Cbuffer(uint32_t scancode) {
    key_buffer[head] = (uint8_t)scancode; // store only low byte
    head++;
    if (head >= BUFFER_SIZE) {
        head = 0; // wrap around
    }
    }*/




void InitScreen(char *video_memory) {
    for (int i = 0; i < VGA_TOTAL_BYTES; i += 2) {
        video_memory[i] = ' ';    // space char
        video_memory[i+1] = 0x07; // attribute byte (color)
    }
}


void yprint(int *cursor_x, char *video_memory, char message[]) {
    for (int i = 0; message[i] != '\0'; i++) {
       if (message[i] == '\n') {
	 int currentLine = *cursor_x / VGA_BYTES_LINE ;
	 *cursor_x = (currentLine+1)*VGA_BYTES_LINE;
	 continue;
         }


        else if (*cursor_x + 1 >= VGA_TOTAL_BYTES) break; // prevent overflow
        else{
	 video_memory[*cursor_x] = message[i];
        video_memory[*cursor_x + 1] = 0x0F; // white on black
        *cursor_x += 2;
	}
       
    }
    
}

void main() {

    char* video_memory = (char*) 0xB8000;
    int cursor_x = 0;
    
    char message[100] = "  yaniOS Shell v0kk\nkk.";
    char message2[100] = "\n welcome\n to ";
     InitScreen(video_memory);
    
    yprint(&cursor_x, video_memory, message2);
  
    yprint(&cursor_x, video_memory, message);
    yprint(&cursor_x, video_memory, "loolxd ");
    yprint(&cursor_x, video_memory, "yes you are right");
    yprint(&cursor_x, video_memory, "hello bro \n");
   
    while(1){
      __asm__ volatile ("hlt");
    };
}
