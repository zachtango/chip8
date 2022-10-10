#include <cstdint>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>

using namespace std;

const unsigned int ROM_START_ADDRESS = 0x200;

class Chip8 {

public:
    uint8_t registers[16]{};
    
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
        // return from a subroutine

        /*
            set the program counter to the address at the top of the stack
        */
    }

};

int main(){

    Chip8 chip8 = Chip8();    
    chip8.readROM("puzzle.rom");

    return 0;
}
