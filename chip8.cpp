#include "chip8.h"

uint8_t fontset[FONTSET_SIZE] = {
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

Chip8::Chip8(){
    rGen = mt19937(time(NULL));
    rByte = uniform_int_distribution<uint8_t>(0, 255);

    delayTimer = 0;
    soundTimer = 0;
    sp = 0;
    pc = ROM_START_ADDRESS;

    memset(video, 0, sizeof(video));

    /*
        0x050 - 0x0A0 reserved for 16 built-in chars (0 through F)
        ROM looks here for those chars
    */
    for(uint8_t i = 0; i < FONTSET_SIZE; i++){
        memory[KEYPAD_START_ADDRESS + i] = fontset[i];
    }

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

void Chip8::Cycle(){
    //I_Fx0A
    // fetch next instruction (the next instr will be at pc)
    opcode = (memory[pc] << 8u) | memory[pc + 1];

    pc += 2;

    uint8_t i = opcode >> 12u; 
    // decode and execute instr
    auto table = this->functions;
    if (table.find(i) != table.end())
        (this->*functions[i])();

    if(delayTimer > 0){
        delayTimer -= 1;
    }

    if(soundTimer > 0){
        soundTimer -= 1;
    }
}

void Chip8::readROM(string fileName){
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
            memory[ROM_START_ADDRESS + i] = buffer[i];
        }

        // free buffer space in memory
        delete [] buffer;
    }
}

// Instr Set

// decodes instrunctions 0, 8, E, F
void Chip8::I_0(){
    uint8_t i = opcode & 0x00FFu;
    auto table = this->functions0;

    if (table.find(i) != table.end()){
        (this->*functions0[i])();
    }
}

void Chip8::I_8(){
    uint8_t i = opcode & 0x000Fu;
    auto table = this->functions8;

    if (table.find(i) != table.end()){
        (this->*functions8[i])();
    }
}

void Chip8::I_E(){
    uint8_t i = opcode & 0x00FFu;
    auto table = this->functionsE;

    if (table.find(i) != table.end()){
        (this->*functionsE[i])();
    }
}

void Chip8::I_F(){
    uint8_t i = opcode & 0x00FFu;
    auto table = this->functionsF;

    if (table.find(i) != table.end()){
        (this->*functionsF[i])();
    }
}

// extract bits
uint8_t Chip8::I_x(){
    return (opcode & 0x0F00u) >> 8u;
}

uint8_t Chip8::I_y(){
    return (opcode & 0x00F0u) >> 4u;
}

uint8_t Chip8::I_kk(){
    return opcode & 0x00FFu;
}

uint16_t Chip8::I_nnn(){
    return opcode & 0x0FFFu;
}

uint8_t Chip8::I_i(){
    return (opcode & 0xF000u) >> 12u;
}

// instructions
void Chip8::I_00E0(){
    // clear the display (CLS)

    // set all bits of video to 0

    memset(video, 0, sizeof(video));
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

    // set the pc to nnn
    pc = I_nnn();
}

void Chip8::I_2nnn(){
    // call subroutine at nnn (CALL addr)

    /*
    increment the sp
    put the current pc on the top of the stk
    set pc to nnn
    */

    stk[sp] = pc;
    sp += 1;

    pc = I_nnn();
}

void Chip8::I_3xkk(){
    // skip the next instr if Vx != kk (SNE Vx, byte) (see V in properties)
    
    /*
    The interpreter compares register Vx to kk, and if they are equal, increments the program counter by 2.
    */

    if(V[I_x()] == I_kk()){
        pc += 2;
    }
}


void Chip8::I_4xkk(){ 
    // SNE Vx, byte
    // Skip next instruction if Vx != kk.

    // compares register Vx to kk, and if they are not equal, increments the program counter by 2.

    if(V[I_x()] != I_kk()){
        pc += 2;
    }
}

void Chip8::I_5xy0(){
    // SE Vx, Vy
    // Skip next instruction if Vx = Vy.

    // compares register Vx to register Vy, and if they are equal, increments the program counter by 2.
    if(V[I_x()] == V[I_y()]){
        pc += 2;
    }
}

void Chip8::I_6xkk(){
    // LD Vx, byte
    // Set Vx = kk.

    // puts the value kk into register Vx.

    V[I_x()] = I_kk();
}

void Chip8::I_7xkk(){
    // ADD Vx, byte
    // Set Vx = Vx + kk.

    // Adds the value kk to the value of register Vx, then stores the result in Vx.
    
    V[I_x()] += I_kk();
}

void Chip8::I_8xy0(){
    // LD Vx, Vy
    // Set Vx = Vy.

    // Stores the value of register Vy in register Vx.
    V[I_x()] = V[I_y()];
}

void Chip8::I_8xy1(){
    // OR Vx, Vy
    // Set Vx = Vx OR Vy.

    /*
    Performs a bitwise OR on the values of Vx and Vy, then stores the result in Vx. 
    */

    V[I_x()] |= V[I_y()];
}

void Chip8::I_8xy2(){
    // AND Vx, Vy
    // Set Vx = Vx AND Vy.

    /*
    Performs a bitwise AND on the values of Vx and Vy
    then stores the result in Vx. 
    */

    V[I_x()] &= V[I_y()];
}

void Chip8::I_8xy3(){
    // XOR Vx, Vy
    // Set Vx = Vx XOR Vy.

    /*
    Performs a bitwise exclusive OR on the values of Vx and Vy
    then stores the result in Vx. 
    */

    V[I_x()] ^= V[I_y()];
}

void Chip8::I_8xy4(){
    // ADD Vx, Vy
    // Set Vx = Vx + Vy, set VF = carry.

    /*
    The values of Vx and Vy are added together. 
    If the result is greater than 8 bits (i.e., > 255,) VF is set to 1, otherwise 0. 
    Only the lowest 8 bits of the result are kept, and stored in Vx.
    */

    uint16_t sum = V[I_x()] + V[I_y()];
    
    if(sum > 255u){
        V[0xF] = 1;
    } else{
        V[0xF] = 0;
    }

    V[I_x()] = sum & 0xFFu; // save last 8 bits
}

void Chip8::I_8xy5(){
    // SUB Vx, Vy
    // Set Vx = Vx - Vy, set VF = NOT borrow.

    // If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, and the results stored in Vx.

    if(V[I_x()] > V[I_y()]){
        V[0xF] = 1;
    } else{
        V[0xF] = 0;
    }

    V[I_x()] -= V[I_y()];
}

void Chip8::I_8xy6(){
    // SHR Vx {, Vy}
    // Set Vx = Vx SHR 1.

    // If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2.
    
    uint8_t lsb = V[I_x()] & 1u;
    
    V[0xF] = lsb;
    V[I_x()] >>= 1;
}

void Chip8::I_8xy7(){
    // SUBN Vx, Vy
    // Set Vx = Vy - Vx, set VF = NOT borrow.

    // If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy, and the results stored in Vx.


    if(V[I_y()] > V[I_x()]){
        V[0xF] = 1;
    } else{
        V[0xF] = 0;
    }

    V[I_x()] = V[I_y()] - V[I_x()];
}

void Chip8::I_8xyE(){ 
    // SHL Vx {, Vy}
    // Set Vx = Vx SHL 1.

    // If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2.

    uint8_t msb = (V[I_x()] & 0x80u) >> 7u;

    V[0xF] = msb;
    V[I_x()] <<= 1;
}

void Chip8::I_9xy0(){ 
    // SNE Vx, Vy
    // Skip next instruction if Vx != Vy.

    // The values of Vx and Vy are compared, and if they are not equal, the program counter is increased by 2.

    if(V[I_x()] != V[I_y()]){
        pc += 2;
    }
}

void Chip8::I_Annn(){
    // I is the index register
    // LD I, addr
    // Set I = nnn.

    // The value of register I is set to nnn.

    index = I_nnn();
}

void Chip8::I_Bnnn(){
    // JP V0, addr
    // Jump to location nnn + V0.

    // The program counter is set to nnn plus the value of V0.

    pc = V[0x0] + I_nnn();
}

void Chip8::I_Cxkk(){
    // RND Vx, byte
    // Set Vx = random byte AND kk.

    // The interpreter generates a random number from 0 to 255, which is then ANDed with the value kk. The results are stored in Vx
    
    uint8_t r = rByte(rGen);
    
    V[I_x()] = r & I_kk();
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
    
    uint8_t n = opcode & 0x000Fu;

    // (Vx, Vy) maps to V[I_y()] * 64 + V[I_x()] index in video
    uint8_t xPos = V[I_x()] % VIDEO_WIDTH;
    uint8_t yPos = V[I_y()] % VIDEO_HEIGHT;

    V[0xF] = 0;

    for(int row = 0; row < n; row++){
        // sprite row i
        uint8_t spriteRow = memory[index + row];

        // start from leftmost bit and go to rightmost bit
        for(uint8_t col = 0; col < 8; col++){ 
            // for each bit in spriteRow, xor it with the corresponding bit in video
            uint8_t spriteBit = (spriteRow & (0x80u >> col));
            uint32_t videoI = (yPos + row) * VIDEO_WIDTH + xPos + col;

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

    if(keypad[V[I_x()]]){
        pc += 2;
    }
}

void Chip8::I_ExA1(){
    // SKNP Vx
    // Skip next instruction if key with the value of Vx is not pressed.

    // Checks the keyboard, and if the key corresponding to the value of Vx is currently in the up position, PC is increased by 2.

    if(!keypad[V[I_x()]]){
        pc += 2;
    }
}

void Chip8::I_Fx07(){
    // LD Vx, DT
    // Set Vx = delay timer value.

    // The value of DT is placed into Vx.

    V[I_x()] = delayTimer;
}


void Chip8::I_Fx0A(){
    // LD Vx, K
    // Wait for a key press, store the value of the key in Vx.

    // All execution stops until a key is pressed, then the value of that key is stored in Vx.


    bool pressed = false;

    for(uint8_t i = 0; i < 16; i++){
        if(keypad[i]){
            V[I_x()] = i;
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
    delayTimer = V[I_x()];
}

void Chip8::I_FX18(){
    // LD ST, Vx
    // Set sound timer = Vx.

    // ST is set equal to the value of Vx.
    soundTimer = V[I_x()];
}

void Chip8::I_Fx1E(){
    // ADD I, Vx
    // Set I = I + Vx.

    // The values of I and Vx are added, and the results are stored in I.
    index = index + V[I_x()];
}

void Chip8::I_Fx29(){
    // LD F, Vx
    // Set I = location of sprite for digit Vx.

    /* 
    The value of I is set to the location for the hexadecimal sprite corresponding to the value of Vx. 
    See section 2.4, Display, for more information on the Chip-8 hexadecimal font.
    */

    index = FONTSET_START_ADDRESS + (5 * V[I_x()]);
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
    uint8_t value = V[I_x()];

    memory[index + 2] = value % 10;
    value /= 10;
    memory[index + 1] = value % 10;
    value /= 10;
    memory[index] = value % 10;
}

void Chip8::I_Fx55(){
    // LD [I], Vx
    // Store registers V0 through Vx in memory starting at location I.

    // The interpreter copies the values of registers V0 through Vx into memory, starting at the address in I.

    for(uint8_t i = 0; i <= I_x(); i++){
        memory[index + i] = V[i];
    }
}

void Chip8::I_Fx65(){
    // LD Vx, [I]
    // Read registers V0 through Vx from memory starting at location I.

    // The interpreter reads values from memory starting at location I into registers V0 through Vx.

    for(uint8_t i = 0; i <= I_x(); i++){
        V[i] = memory[index + i];
    }
}

