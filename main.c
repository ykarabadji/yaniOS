// main.c
void main() {
    // Print an 'X' to the screen to confirm the C code is running
    char* video_memory = (char*) 0xb8000;
  
    for(int i =0;i<4000;i++){

      *(video_memory + i) = 0;
    *(video_memory + i+1) = 0x07; // White on black
    }}
