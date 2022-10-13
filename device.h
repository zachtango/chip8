#pragma once

#include <SDL2/SDL.h>

using namespace std;

class Device{

private:
    SDL_Window *window;
    SDL_Texture *texture;
    SDL_Renderer *renderer;

public:
    Device();
    ~Device();

    void updateWindow(void * video, int rowBits);
    bool processInput(uint8_t *keypad);
};
