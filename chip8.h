#pragma once

#include <cstdint>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <time.h>
#include <random>
#include <unordered_map>
#include <chrono>

using namespace std;

#define ROM_START_ADDRESS 0x200
#define VIDEO_WIDTH 64
#define VIDEO_HEIGHT 32
#define KEYPAD_START_ADDRESS 0x050



class Chip8 {

public:
    typedef void (Chip8::*Chip8Func)();
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
    uint32_t video[VIDEO_WIDTH * VIDEO_HEIGHT]{}; // 0x00000000 or 0xFFFFFFFF
    uint16_t opcode{}; // stores curr instr
    uniform_int_distribution<uint8_t> rByte;
    mt19937 rGen;
    unordered_map<uint16_t, Chip8Func> functions; // function table that points to function
    unordered_map<uint16_t, Chip8Func> functions0;
    unordered_map<uint16_t, Chip8Func> functions8;
    unordered_map<uint16_t, Chip8Func> functionsE;
    unordered_map<uint16_t, Chip8Func> functionsF;

    Chip8();

    void readROM(string fileName);

    // Instr Set
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

    void I_0();

    void I_8();

    void I_E();

    void I_F();

    void Cycle();
};