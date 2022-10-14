#pragma once

#include <cstdint>
#include <unordered_map>
#include <random>
#include <fstream>
#include <time.h>
#include <cstring>

using namespace std;

#define ROM_START_ADDRESS 0x200
#define VIDEO_WIDTH 64
#define VIDEO_HEIGHT 32
#define KEYPAD_START_ADDRESS 0x050
#define FONTSET_SIZE 80
#define FONTSET_START_ADDRESS 0x080
#define KEY_COUNT 16
#define REGISTER_COUNT 16
#define MEMORY_SIZE 4096
#define STACK_LEVELS 16

class Chip8 {

public:
    /*
        0x000 - 0x1FF reserved for CHIP-8 interpreter (don't read/write from)

        0x050 - 0x0A0 reserved for 16 built-in chars (0 through F)
            ROM looks here for those chars

        0x200 - 0xFFF : 512 - 4095 in decimal
            ROM instr stored starting at 0x200
            anything left after the ROM's space is free memory to use
    */
    uint8_t keypad[KEY_COUNT];
    uint32_t video[VIDEO_WIDTH * VIDEO_HEIGHT]; // 0x00000000 or 0xFFFFFFFF

    Chip8();

    void readROM(string fileName);

    void Cycle();

private:
    uint8_t V[REGISTER_COUNT]{}; // Vx refers to a register where x is the hex digit 0 to F (16 registers total)
    uint8_t memory[MEMORY_SIZE];
    uint16_t stk[STACK_LEVELS]{};
    uint16_t index{};
    uint16_t pc{};
    uint8_t sp{};
    uint8_t delayTimer{};
    uint8_t soundTimer{};
    uint16_t opcode{}; // stores curr instr
    uniform_int_distribution<uint8_t> rByte;
    mt19937 rGen;

    // function tables that points to the function associated with the instruction in the opcode
    typedef void (Chip8::*Chip8Func)();
    unordered_map<uint16_t, Chip8Func> functions; 
    unordered_map<uint16_t, Chip8Func> functions0;
    unordered_map<uint16_t, Chip8Func> functions8;
    unordered_map<uint16_t, Chip8Func> functionsE;
    unordered_map<uint16_t, Chip8Func> functionsF;

    // Instr Set

    // extracts bits
    uint8_t I_x();
    uint8_t I_y();
    uint8_t I_kk();
    uint16_t I_nnn();
    uint8_t I_i();

    // decodes instr starting w/ 0, 8, E, and F
    void I_0();
    void I_8();
    void I_E();
    void I_F();

    // instructions
    void I_00E0();
    void I_00EE();
    void I_1nnn();
    void I_2nnn();
    void I_3xkk();
    void I_4xkk();
    void I_5xy0();
    void I_6xkk();
    void I_7xkk();
    void I_8xy0();
    void I_8xy1();
    void I_8xy2();
    void I_8xy3();
    void I_8xy4();
    void I_8xy5();
    void I_8xy6();
    void I_8xy7();
    void I_8xyE();
    void I_9xy0();
    void I_Annn();
    void I_Bnnn();
    void I_Cxkk();
    void I_Dxyn();
    void I_Ex9E();
    void I_ExA1();
    void I_Fx07();
    void I_Fx0A();
    void I_Fx15();
    void I_FX18();
    void I_Fx1E();
    void I_Fx29();
    void I_Fx33();
    void I_Fx55();
    void I_Fx65();

};
