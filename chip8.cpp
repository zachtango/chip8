#include <cstdint>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>

using namespace std;

const unsigned int ROM_START_ADDRESS = 0x200;

class Chip8 {

public:
    uint8_t V[16]{}; // Vx refers to a register where x is the hex digit 0 to F (16 registers total)
    
    /*
        0x000 - 0x1FF reserved for CHIP-8 interpreter (don't read/write from)

        0x050 - 0x0A0 reserved for 16 built-in chars (0 through F)
            ROM looks here for those chars

        0x200 - 0xFFF : 512 - 4095 in decimal
            ROM instr stored starting at 0x200
            anything left after the ROM's space is free memory to use
    */
    uint8_t memory[4096]{};

    uint16_t index{};
    uint16_t pc{};
    uint8_t stk[16]{};
    uint8_t sp{};
    uint8_t delayTimer{};
    uint8_t soundTimer{};
    uint8_t keypad[16]{};
    uint32_t video[64 * 32]{};
    uint16_t opcode{};

    void readROM(string fileName){
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
    void I_00E0(){
        // clear the display (CLS)

    }

    void I_00EE(){
        // return from a subroutine (RET)

        /*
        set the program counter to the address at the top of the stack
        subtracts 1 from the stack pointer
        */

        pc = stk[sp];
        sp -= 1;
    }

    void I_1nnn(uint16_t nnn){
        // jump to location nnn (JP addr)

        // set the pc to nnn
        pc = nnn;
    }

    void I_2nnn(uint16_t nnn){
        // call subroutine at nnn (CALL addr)

        /*
        increment the sp
        put the current pc on the top of the stk
        set pc to nnn
        */

        sp += 1;
        stk[sp] = pc;
        pc = nnn;
    }

    void I_3xkk(int x, uint8_t kk){
        // skip the next instr if Vx != kk (SNE Vx, byte) (see V in properties)
        
        /*
        The interpreter compares register Vx to kk, and if they are equal, increments the program counter by 2.
        */

        if(V[x] == kk){
            pc += 2;
        }
    }

    
    void I_4xkk(int x, uint8_t kk){ 
        // SNE Vx, byte
        // Skip next instruction if Vx != kk.

        // compares register Vx to kk, and if they are not equal, increments the program counter by 2.

        if(V[x] != kk){
            pc += 2;
        }
    }

    void I_5xy0(int x, int y){
        // SE Vx, Vy
        // Skip next instruction if Vx = Vy.

        // compares register Vx to register Vy, and if they are equal, increments the program counter by 2.
        if(V[x] == V[y]){
            pc += 2;
        }
    }

    void I_6xkk(int x, uint8_t kk){
        // LD Vx, byte
        // Set Vx = kk.

        // puts the value kk into register Vx.
        V[x] = kk;
    }

    void I_7xkk(int x, uint8_t kk){
        // ADD Vx, byte
        // Set Vx = Vx + kk.

        // Adds the value kk to the value of register Vx, then stores the result in Vx.
    
        V[x] += kk;
    }

    void I_8xy0(int x, int y){
        // LD Vx, Vy
        // Set Vx = Vy.

        // Stores the value of register Vy in register Vx.
        V[x] = V[y];
    }

    void I_8xy1(int x, int y){
        // OR Vx, Vy
        // Set Vx = Vx OR Vy.

        /*
        Performs a bitwise OR on the values of Vx and Vy, then stores the result in Vx. 
        */

        V[x] = V[x] | V[y];
    }

    void I_8xy2(int x, int y){
        // AND Vx, Vy
        // Set Vx = Vx AND Vy.

        /*
        Performs a bitwise AND on the values of Vx and Vy
        then stores the result in Vx. 
        */

        V[x] = V[x] & V[y];
    }

    void I_8xy3(int x, int y){
        // XOR Vx, Vy
        // Set Vx = Vx XOR Vy.

        /*
        Performs a bitwise exclusive OR on the values of Vx and Vy
        then stores the result in Vx. 
        */

        V[x] = V[x] ^ V[y];
    }

    void I_8xy4(int x, int y){
        // ADD Vx, Vy
        // Set Vx = Vx + Vy, set VF = carry.

        /*
        The values of Vx and Vy are added together. 
        If the result is greater than 8 bits (i.e., > 255,) VF is set to 1, otherwise 0. 
        Only the lowest 8 bits of the result are kept, and stored in Vx.
        */

        int sum = V[x] + V[y];
        
        if(sum > 255){
            sum = sum & 0xFF; // save last 8 bits
            V[0xF] = 1;
        } else{
            V[0xF] = 0;
        }

        V[x] = sum;
    }

    void I_8xy5(int x, int y){
        // SUB Vx, Vy
        // Set Vx = Vx - Vy, set VF = NOT borrow.

        // If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, and the results stored in Vx.

        if(V[x] > V[y]){
            V[0xF] = 1;
        } else{
            V[0xF] = 0;
        }

        V[x] = V[x] - V[y];
    }

    void I_8xy6(int x){
        // SHR Vx {, Vy}
        // Set Vx = Vx SHR 1.

        // If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2.

        int lsb = V[x] & 1;
        
        V[0xF] = lsb;
        V[x] = V[x] >> 1;
    }

    void I_8xy7(int x, int y){
        // SUBN Vx, Vy
        // Set Vx = Vy - Vx, set VF = NOT borrow.

        // If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy, and the results stored in Vx.

        if(V[y] > V[x]){
            V[0xF] = 1;
        } else{
            V[0xF] = 0;
        }

        V[x] = V[y] - V[x];
    }

    void I_8xyE(int x){ 
        // SHL Vx {, Vy}
        // Set Vx = Vx SHL 1.

        // If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2.
        int msb = V[x] >> 7;

        V[0xF] = msb;
        V[x] = V[x] << 1;
    }

    void I_9xy0(int x, int y){ 
        // SNE Vx, Vy
        // Skip next instruction if Vx != Vy.

        // The values of Vx and Vy are compared, and if they are not equal, the program counter is increased by 2.
        if(V[x] != V[y]){
            pc += 2;
        }
    }

    void I_Annn(uint16_t nnn){
        // I is the index register
        // LD I, addr
        // Set I = nnn.

        // The value of register I is set to nnn.

        index = nnn;
    }

    void I_Bnnn(uint16_t nnn){
        // JP V0, addr
        // Jump to location nnn + V0.

        // The program counter is set to nnn plus the value of V0.

        pc = V[0] + nnn;
    }

    void I_Cxkk(){
        // RND Vx, byte
        // Set Vx = random byte AND kk.

        // The interpreter generates a random number from 0 to 255, which is then ANDed with the value kk. The results are stored in Vx

    }

    void I_Dxyn(){
        // DRW Vx, Vy, nibble
        // Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.

        /*
        The interpreter reads n bytes from memory, starting at the address stored in I. 
        These bytes are then displayed as sprites on screen at coordinates (Vx, Vy). 
        Sprites are XORed onto the existing screen. If this causes any pixels to be erased, VF is set to 1, otherwise it is set to 0. 
        If the sprite is positioned so part of it is outside the coordinates of the display, it wraps around to the opposite side of the screen. 
        */
    }

    void I_Ex9E(){
        // SKP Vx
        // Skip next instruction if key with the value of Vx is pressed.

        // Checks the keyboard, and if the key corresponding to the value of Vx is currently in the down position, PC is increased by 2.
    }

    /*
    ExA1 - SKNP Vx
    Skip next instruction if key with the value of Vx is not pressed.

    Checks the keyboard, and if the key corresponding to the value of Vx is currently in the up position, PC is increased by 2.


    Fx07 - LD Vx, DT
    Set Vx = delay timer value.

    The value of DT is placed into Vx.


    Fx0A - LD Vx, K
    Wait for a key press, store the value of the key in Vx.

    All execution stops until a key is pressed, then the value of that key is stored in Vx.


    Fx15 - LD DT, Vx
    Set delay timer = Vx.

    DT is set equal to the value of Vx.


    Fx18 - LD ST, Vx
    Set sound timer = Vx.

    ST is set equal to the value of Vx.


    Fx1E - ADD I, Vx
    Set I = I + Vx.

    The values of I and Vx are added, and the results are stored in I.


    Fx29 - LD F, Vx
    Set I = location of sprite for digit Vx.

    The value of I is set to the location for the hexadecimal sprite corresponding to the value of Vx. See section 2.4, Display, for more information on the Chip-8 hexadecimal font.
    */

    void I_Fx33(int x){
        // LD B, Vx
        // Store BCD representation of Vx in memory locations I, I+1, and I+2.

        /*
        The interpreter takes the decimal value of Vx
        and places the hundreds digit in memory at location in I
        the tens digit at location I+1
        and the ones digit at location I+2.
        */

        memory[index] = (V[x] / 100) % 10;
        memory[index + 1] = (V[x] / 10) % 10;
        memory[index + 2] = (V[x]) % 10;
    }

    void I_Fx55(int x){
        // LD [I], Vx
        // Store registers V0 through Vx in memory starting at location I.

        // The interpreter copies the values of registers V0 through Vx into memory, starting at the address in I.

        for(int i = 0; i < x + 1; i++){
            memory[index + i] = V[i];
        }
    }

    void I_Fx65(int x){
        // LD Vx, [I]
        // Read registers V0 through Vx from memory starting at location I.

        // The interpreter reads values from memory starting at location I into registers V0 through Vx.

        for(int i = 0; i < x + 1; i++){
            V[i] = memory[index + i];
        }
    }

};

int main(){

    Chip8 chip8 = Chip8();    
    chip8.readROM("puzzle.rom");

    return 0;
}
