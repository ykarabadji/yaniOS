

/*ubyte readPort60() {
    ubyte value;
    asm {
        mov DX, 0x60      ; port number
        in  AL, DX        ; read from port into AL
        mov value, AL     ; store result in D variable
    }
    return value;
}
extern(C) void vga_write(char c);



void main(){

}
void handleKey(ubyte scanCode) {
    vga_write(scanCodeToChar(scanCode)); // convert scan code to char
}
*/

import std.stdio;

void main() {
    while (true) {
        char c = cast(char)stdin.readln()[0]; // read one character
        write(c); // print it
    }
}
