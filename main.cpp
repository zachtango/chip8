#include "chip8.h"
#include "device.h"

using namespace std;


int main(int argc, char *argv[]){
    if(argc != 2){
        cout << "chip8.exe <rom file>\n";
        return 0;
    }

    Chip8 chip8 = Chip8();
    Device device = Device();
    uint32_t video[VIDEO_WIDTH * VIDEO_HEIGHT];

    chip8.readROM(string(argv[1]));

    int cycleDelay = 3; // order of ms
    auto prevCycle = std::chrono::steady_clock::now();
    int rowSize = VIDEO_WIDTH * sizeof(chip8.video[0]);

    bool quit = false;
    while(!quit){
        quit = device.processInput(chip8.keypad);
        auto currTime = std::chrono::steady_clock::now();
        double elapsedMS = chrono::duration<double, milli>(currTime - prevCycle).count();
        
        if( elapsedMS > cycleDelay ){
            prevCycle = chrono::steady_clock::now();
            chip8.Cycle();
            device.updateWindow(chip8.video, rowSize);
        }
    }

    return 0;
}