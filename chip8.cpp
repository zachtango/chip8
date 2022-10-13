#include "chip8.h"
#include <cstdint>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <time.h>
#include <random>
#include <unordered_map>
#include <chrono>

#define ROM_START_ADDRESS 0x200
#define VIDEO_WIDTH 64
#define VIDEO_HEIGHT 32
#define KEYPAD_START_ADDRESS 0x050

Chip8::Chip8(){
    // srand(time(NULL)); // rand seed
    rGen = mt19937(time(NULL));
    rByte = uniform_int_distribution<uint8_t>(0, 255);

    pc = ROM_START_ADDRESS;

    /*
    0x050 - 0x0A0 reserved for 16 built-in chars (0 through F)
    ROM looks here for those chars
    */

    uint8_t keyValues[16 * 5] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0,
        0x20, 0x60, 0x20, 0x20, 0x70, 
        0xF0, 0x10, 0xF0, 0x80, 0xF0, 
        0xF0, 0x10, 0xF0, 0x10, 0xF0, 
        0x90, 0x90, 0xF0, 0x10, 0x10, 
        0xF0, 0x80, 0xF0, 0x10, 0xF0, 
        0xF0, 0x80, 0xF0, 0x90, 0xF0, 
        0xF0, 0x10, 0x20, 0x40, 0x40,
        0xF0, 0x90, 0xF0, 0x90, 0xF0,
        0xF0, 0x90, 0xF0, 0x10, 0xF0,
        0xF0, 0x90, 0xF0, 0x90, 0x90,
        0xE0, 0x90, 0xE0, 0x90, 0xE0,
        0xF0, 0x80, 0x80, 0x80, 0xF0,
        0xE0, 0x90, 0x90, 0x90, 0xE0,
        0xF0, 0x80, 0xF0, 0x80, 0xF0,
        0xF0, 0x80, 0xF0, 0x80, 0x80
    };

    for(uint8_t i = 0; i < 16 * 5; i++){
        memory[KEYPAD_START_ADDRESS + i] = keyValues[i];
    }

    // functions alrdy intitialized?? idk
    functions[0x0] = &I_0;
    functions[0x1] = &I_1nnn;
    functions[0x2] = &I_2nnn;
    functions[0x3] = &I_3xkk;
    functions[0x4] = &I_4xkk;
    functions[0x5] = &I_5xy0;
    functions[0x6] = &I_6xkk;
    functions[0x7] = &I_7xkk;
    functions[0x8] = &I_8;
    functions[0x9] = &I_9xy0;
    functions[0xA] = &I_Annn;
    functions[0xB] = &I_Bnnn;
    functions[0xC] = &I_Cxkk;
    functions[0xD] = &I_Dxyn;
    functions[0xE] = &I_E;
    functions[0xF] = &I_F;
    // 0x0
    functions0[0xE0] = &I_00E0;
    functions0[0xEE] = &I_00EE;
    // 0x8
    functions8[0x0] = &I_8xy0;
    functions8[0x1] = &I_8xy1;
    functions8[0x2] = &I_8xy2;
    functions8[0x3] = &I_8xy3;
    functions8[0x4] = &I_8xy4;
    functions8[0x5] = &I_8xy5;
    functions8[0x6] = &I_8xy6;
    functions8[0x7] = &I_8xy7;
    functions8[0xE] = &I_8xyE;
    // 0xE
    functionsE[0x9E] = &I_Ex9E;
    functionsE[0xA1] = &I_ExA1;
    // 0xF
    functionsF[0x07] = &I_Fx07; 
    functionsF[0x0A] = &I_Fx0A;
    functionsF[0x15] = &I_Fx15;
    functionsF[0x18] = &I_FX18;
    functionsF[0x1E] = &I_Fx1E;
    functionsF[0x29] = &I_Fx29;
    functionsF[0x33] = &I_Fx33;
    functionsF[0x55] = &I_Fx55;
    functionsF[0x65] = &I_Fx65;
}

void Chip8::readROM(string fileName){
    memory[ROM_START_ADDRESS] = 0xF0;
    memory[ROM_START_ADDRESS + 1] = 0x0A;
    return;

    ifstream file;

    file.open(fileName, ios::binary | ios::ate);

    if( file.is_open() ){
        // get file size
        streampos size = file.tellg();
        char * buffer = new char[size];

        // load file instr into buffer
        file.seekg(0, ios::beg);
        file.read(buffer, size);
        file.close();

        // enter instr from buffer into memory
        for(long i = 0; i < size; i++){
            memory[ROM_START_ADDRESS + i] = (* (buffer + i));
        }

        // free buffer space in memory
        delete [] buffer;
    }
}


// Instr Set
void Chip8::I_00E0(){
    // clear the display (CLS)

    // set all bits of video to 0

    for(int i = 0; i < 64 * 32; i++){
        video[i] = 0;
    }
}

void Chip8::I_00EE(){
    // return from a subroutine (RET)

    /*
    set the program counter to the address at the top of the stack
    subtracts 1 from the stack pointer
    */

    sp -= 1;
    pc = stk[sp];
}

void Chip8::I_1nnn(){
    // jump to location nnn (JP addr)

    uint16_t addr = opcode & 0x0FFFu;
    // set the pc to nnn
    pc = addr;
}

void Chip8::I_2nnn(){
    // call subroutine at nnn (CALL addr)

    /*
    increment the sp
    put the current pc on the top of the stk
    set pc to nnn
    */
    uint16_t addr = opcode & 0x0FFFu;

    stk[sp] = pc;
    sp += 1;
    
    pc = addr;
}

void Chip8::I_3xkk(){
    // skip the next instr if Vx != kk (SNE Vx, byte) (see V in properties)
    
    /*
    The interpreter compares register Vx to kk, and if they are equal, increments the program counter by 2.
    */

    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t kk = (opcode & 0x00FFu);

    if(V[x] == kk){
        pc += 2;
    }
}


void Chip8::I_4xkk(){ 
    // SNE Vx, byte
    // Skip next instruction if Vx != kk.

    // compares register Vx to kk, and if they are not equal, increments the program counter by 2.

    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t kk = (opcode & 0x00FFu);

    if(V[x] != kk){
        pc += 2;
    }
}

void Chip8::I_5xy0(){
    // SE Vx, Vy
    // Skip next instruction if Vx = Vy.

    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t y = (opcode & 0x00F0u) >> 4u;

    // compares register Vx to register Vy, and if they are equal, increments the program counter by 2.
    if(V[x] == V[y]){
        pc += 2;
    }
}

void Chip8::I_6xkk(){
    // LD Vx, byte
    // Set Vx = kk.

    // puts the value kk into register Vx.

    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t kk = opcode & 0x00FFu;

    V[x] = kk;
}

void Chip8::I_7xkk(){
    // ADD Vx, byte
    // Set Vx = Vx + kk.

    // Adds the value kk to the value of register Vx, then stores the result in Vx.

    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t kk = opcode & 0x00FFu;
    
    V[x] += kk;
}

void Chip8::I_8xy0(){
    // LD Vx, Vy
    // Set Vx = Vy.

    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t y = (opcode & 0x00F0u) >> 4u;

    // Stores the value of register Vy in register Vx.
    V[x] = V[y];
}

void Chip8::I_8xy1(){
    // OR Vx, Vy
    // Set Vx = Vx OR Vy.

    /*
    Performs a bitwise OR on the values of Vx and Vy, then stores the result in Vx. 
    */

    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t y = (opcode & 0x00F0u) >> 4u;

    V[x] = V[x] | V[y];
}

void Chip8::I_8xy2(){
    // AND Vx, Vy
    // Set Vx = Vx AND Vy.

    /*
    Performs a bitwise AND on the values of Vx and Vy
    then stores the result in Vx. 
    */

    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t y = (opcode & 0x00F0u) >> 4u;

    V[x] = V[x] & V[y];
}

void Chip8::I_8xy3(){
    // XOR Vx, Vy
    // Set Vx = Vx XOR Vy.

    /*
    Performs a bitwise exclusive OR on the values of Vx and Vy
    then stores the result in Vx. 
    */

    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t y = (opcode & 0x00F0u) >> 4u;

    V[x] = V[x] ^ V[y];
}

void Chip8::I_8xy4(){
    // ADD Vx, Vy
    // Set Vx = Vx + Vy, set VF = carry.

    /*
    The values of Vx and Vy are added together. 
    If the result is greater than 8 bits (i.e., > 255,) VF is set to 1, otherwise 0. 
    Only the lowest 8 bits of the result are kept, and stored in Vx.
    */

    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t y = (opcode & 0x00F0u) >> 4u;

    uint16_t sum = V[x] + V[y];
    
    if(sum > 255u){
        V[0xF] = 1;
    } else{
        V[0xF] = 0;
    }

    V[x] = sum & 0xFFu; // save last 8 bits
}

void Chip8::I_8xy5(){
    // SUB Vx, Vy
    // Set Vx = Vx - Vy, set VF = NOT borrow.

    // If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, and the results stored in Vx.

    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t y = (opcode & 0x00F0u) >> 4u;

    if(V[x] > V[y]){
        V[0xF] = 1;
    } else{
        V[0xF] = 0;
    }

    V[x] = V[x] - V[y];
}

void Chip8::I_8xy6(){
    // SHR Vx {, Vy}
    // Set Vx = Vx SHR 1.

    // If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2.

    uint8_t x = (opcode & 0x0F00u) >> 8u;
    
    uint8_t lsb = V[x] & 1u;
    
    V[0xF] = lsb;
    V[x] = V[x] >> 1u;
}

void Chip8::I_8xy7(){
    // SUBN Vx, Vy
    // Set Vx = Vy - Vx, set VF = NOT borrow.

    // If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy, and the results stored in Vx.
    
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t y = (opcode & 0x00F0u) >> 4u;

    if(V[y] > V[x]){
        V[0xF] = 1;
    } else{
        V[0xF] = 0;
    }

    V[x] = V[y] - V[x];
}

void Chip8::I_8xyE(){ 
    // SHL Vx {, Vy}
    // Set Vx = Vx SHL 1.

    // If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2.
    
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    
    uint8_t msb = V[x] >> 7u;

    V[0xF] = msb;
    V[x] = V[x] << 1u;
}

void Chip8::I_9xy0(){ 
    // SNE Vx, Vy
    // Skip next instruction if Vx != Vy.

    // The values of Vx and Vy are compared, and if they are not equal, the program counter is increased by 2.

    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t y = (opcode & 0x00F0u) >> 4u;

    if(V[x] != V[y]){
        pc += 2;
    }
}

void Chip8::I_Annn(){
    // I is the index register
    // LD I, addr
    // Set I = nnn.

    // The value of register I is set to nnn.

    uint16_t nnn = opcode & 0x0FFF;

    index = nnn;
}

void Chip8::I_Bnnn(){
    // JP V0, addr
    // Jump to location nnn + V0.

    // The program counter is set to nnn plus the value of V0.

    uint16_t nnn = opcode & 0x0FFF;

    pc = V[0] + nnn;
}

void Chip8::I_Cxkk(){
    // RND Vx, byte
    // Set Vx = random byte AND kk.

    // The interpreter generates a random number from 0 to 255, which is then ANDed with the value kk. The results are stored in Vx
    
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t kk = (opcode & 0x00FFu);
    
    uint8_t r = rByte(rGen);
    
    V[x] = r & kk;
}

void Chip8::I_Dxyn(){
    // DRW Vx, Vy, nibble
    // Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.

    /*
    The interpreter reads n bytes from memory, starting at the address stored in I. 
    These bytes are then displayed as sprites on screen at coordinates (Vx, Vy). 
    Sprites are XORed onto the existing screen. If this causes any pixels to be erased, VF is set to 1, otherwise it is set to 0. 
    If the sprite is positioned so part of it is outside the coordinates of the display, it wraps around to the opposite side of the screen. 
    */

    /*
        0 - 63
        64 - 127
        128 - 191
        192 - 255
        .
        .
        .
        32 times
    */
    

    // FIXME STILL NEED TO IMPLEMENT WRAPPING
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t y = (opcode & 0x00F0u) >> 4u;
    uint8_t n = opcode & 0x000F;

    // (Vx, Vy) maps to V[y] * 64 + V[x] index in video

    V[0xF] = 0;

    for(int row = 0; row < n; row++){
        // sprite row i
        uint8_t spriteRow = memory[index + row];

        // start from leftmost bit and go to rightmost bit
        for(uint8_t col = 0; col < 8; col++){ 
            // for each bit in spriteRow, xor it with the corresponding bit in video
            uint8_t spriteBit = (spriteRow >> (8 - col - 1)) & 0x1u;
            uint16_t videoI = (V[y] + row) * 64 + V[x] + col;

            if(spriteBit){
                if(video[videoI] == 0xFFFFFFFF) { // collision
                    V[0xF] = 1;
                }
                video[videoI] ^= 0xFFFFFFFF;
            }
        }
    }

}

void Chip8::I_Ex9E(){
    // SKP Vx
    // Skip next instruction if key with the value of Vx is pressed.

    // Checks the keyboard, and if the key corresponding to the value of Vx is currently in the down position, PC is increased by 2.
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    
    if(keypad[V[x]]){
        pc += 2;
    }
}

void Chip8::I_ExA1(){
    // SKNP Vx
    // Skip next instruction if key with the value of Vx is not pressed.

    // Checks the keyboard, and if the key corresponding to the value of Vx is currently in the up position, PC is increased by 2.
    uint8_t x = (opcode & 0x0F00u) >> 8u;

    if(!keypad[V[x]]){
        pc += 2;
    }
}

void Chip8::I_Fx07(){
    // LD Vx, DT
    // Set Vx = delay timer value.

    // The value of DT is placed into Vx.
    uint8_t x = (opcode & 0x0F00u) >> 8u;

    V[x] = delayTimer;
}


void Chip8::I_Fx0A(){
    // LD Vx, K
    // Wait for a key press, store the value of the key in Vx.

    // All execution stops until a key is pressed, then the value of that key is stored in Vx.

    uint8_t x = (opcode & 0x0F00u) >> 8u;
    
    bool pressed = false;

    for(uint8_t i = 0; i < 16; i++){
        if(keypad[i]){
            V[x] = i;
            pressed = true;
            break;
        }
    }

    if(!pressed){
        pc -= 2;
    }
}

void Chip8::I_Fx15(){ 
    // LD DT, Vx
    // Set delay timer = Vx.

    // DT is set equal to the value of Vx.
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    delayTimer = V[x];
}

void Chip8::I_FX18(){
    // LD ST, Vx
    // Set sound timer = Vx.

    // ST is set equal to the value of Vx.
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    soundTimer = V[x];
}

void Chip8::I_Fx1E(){
    // ADD I, Vx
    // Set I = I + Vx.

    // The values of I and Vx are added, and the results are stored in I.
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    index = index + V[x];
}

void Chip8::I_Fx29(){
    // LD F, Vx
    // Set I = location of sprite for digit Vx.

    /* 
    The value of I is set to the location for the hexadecimal sprite corresponding to the value of Vx. 
    See section 2.4, Display, for more information on the Chip-8 hexadecimal font.
    */

    uint8_t x = (opcode & 0x0F00u) >> 8u;

    index = 0x050 + 5 * V[x];
}

void Chip8::I_Fx33(){
    // LD B, Vx
    // Store BCD representation of Vx in memory locations I, I+1, and I+2.

    /*
    The interpreter takes the decimal value of Vx
    and places the hundreds digit in memory at location in I
    the tens digit at location I+1
    and the ones digit at location I+2.
    */

    uint8_t x = (opcode & 0x0F00u) >> 8u;

    memory[index] = (V[x] / 100) % 10;
    memory[index + 1] = (V[x] / 10) % 10;
    memory[index + 2] = (V[x]) % 10;
}

void Chip8::I_Fx55(){
    // LD [I], Vx
    // Store registers V0 through Vx in memory starting at location I.

    // The interpreter copies the values of registers V0 through Vx into memory, starting at the address in I.
    uint8_t x = (opcode & 0x0F00u) >> 8u;

    for(uint8_t i = 0; i <= x; i++){
        memory[index + i] = V[i];
    }
}

void Chip8::I_Fx65(){
    // LD Vx, [I]
    // Read registers V0 through Vx from memory starting at location I.

    // The interpreter reads values from memory starting at location I into registers V0 through Vx.
    uint8_t x = (opcode & 0x0F00u) >> 8u;

    for(uint8_t i = 0; i <= x; i++){
        V[i] = memory[index + i];
    }
}

void Chip8::I_0(){
    (this->*functions0[opcode & 0xFF])();
}

void Chip8::I_8(){
    (this->*functions8[opcode & 0xF])();
}

void Chip8::I_E(){
    (this->*functionsE[opcode & 0xFF])();
}

void Chip8::I_F(){
    (this->*functionsF[opcode & 0xFF])();
}

void Chip8::Cycle(){
    // fetch next instruction (the next instr will be at pc)
    opcode = (memory[pc] << 8u) | memory[pc + 1];
    
    pc += 2;

    // decode and execute instr
    (this->*functions[opcode >> 12u])();

    if(delayTimer > 0){
        delayTimer -= 1;
    }

    if(soundTimer > 0){
        delayTimer -= 1;
    }
}