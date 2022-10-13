#include "chip8.h"
#include "device.h"

using namespace std;


int main(int argv, char** args){
    Chip8 chip8 = Chip8();
    Device device = Device();

    chip8.readROM("puzzle.rom");

    int cycleDelay = 1; // order of ms
    auto prevCycle = std::chrono::steady_clock::now();

    bool quit = false;
    while(!quit){
        quit = device.processInput(chip8.keypad);
        auto currTime = std::chrono::steady_clock::now();
        double elapsedMS = chrono::duration<double, milli>(currTime - prevCycle).count();
        
        if( elapsedMS > cycleDelay ){
            chip8.Cycle();
            prevCycle = chrono::steady_clock::now();
            device.updateWindow(chip8.video, 64 * sizeof(chip8.video[0]));
        }
    }

    return 0;
}