#include <SDL2/SDL.h>

using namespace std;

class Device{

    SDL_Window *window = nullptr;
    SDL_Texture *texture = nullptr;
    SDL_Renderer *renderer = nullptr;


    Device(){
        window = SDL_CreateWindow("SDL Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 320, SDL_WINDOW_SHOWN);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
    }

    ~Device(){
        SDL_DestroyWindow(window);
        window = nullptr;

        SDL_DestroyTexture(texture);
        texture = nullptr;

        SDL_DestroyRenderer(renderer);
        renderer = nullptr;

        SDL_Quit();
    }

    void updateWindow(void * video, int rowBits){
        SDL_UpdateTexture(texture, NULL, video, rowBits);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    bool processInput(uint8_t *keypad){
        /*
            Keypad       Keyboard
            +-+-+-+-+    +-+-+-+-+
            |1|2|3|C|    |1|2|3|4|
            +-+-+-+-+    +-+-+-+-+
            |4|5|6|D|    |Q|W|E|R|
            +-+-+-+-+ => +-+-+-+-+
            |7|8|9|E|    |A|S|D|F|
            +-+-+-+-+    +-+-+-+-+
            |A|0|B|F|    |Z|X|C|V|
            +-+-+-+-+    +-+-+-+-+
        */
        bool quit = false;
        SDL_Event evt;

        while(SDL_PollEvent(&evt) != 0){
            if(evt.type == SDL_QUIT){
                quit = true;
            } else if(evt.type == SDL_KEYDOWN){
                switch(evt.key.keysym.sym){
                    case SDLK_1:
                        keypad[1] = 1;
                        break;
                    case SDLK_2:
                        keypad[2] = 1;
                        break;
                    case SDLK_3:
                        keypad[3] = 1;
                        break;
                    case SDLK_4:
                        keypad[0xC] = 1;
                        break;
                    case SDLK_q:
                        keypad[4] = 1;
                        break;
                    case SDLK_w:
                        keypad[5] = 1;
                        break;
                    case SDLK_e:
                        keypad[6] = 1;
                        break;
                    case SDLK_r:
                        keypad[0xD] = 1;
                        break;
                    case SDLK_a:
                        keypad[7] = 1;
                        break;
                    case SDLK_s:
                        keypad[8] = 1;
                        break;
                    case SDLK_d:
                        keypad[9] = 1;
                        break;
                    case SDLK_f:
                        keypad[0xE] = 1;
                        break;
                    case SDLK_z:
                        keypad[0xA] = 1;
                        break;
                    case SDLK_x:
                        keypad[0] = 1;
                        break;
                    case SDLK_c:
                        keypad[0xB] = 1;
                        break;
                    case SDLK_v:
                        keypad[0xF] = 1;
                        break;
                }
            } else if(evt.type == SDL_KEYUP){
                switch(evt.key.keysym.sym){
                    case SDLK_1:
                        keypad[1] = 0;
                        break;
                    case SDLK_2:
                        keypad[2] = 0;
                        break;
                    case SDLK_3:
                        keypad[3] = 0;
                        break;
                    case SDLK_4:
                        keypad[0xC] = 0;
                        break;
                    case SDLK_q:
                        keypad[4] = 0;
                        break;
                    case SDLK_w:
                        keypad[5] = 0;
                        break;
                    case SDLK_e:
                        keypad[6] = 0;
                        break;
                    case SDLK_r:
                        keypad[0xD] = 0;
                        break;
                    case SDLK_a:
                        keypad[7] = 0;
                        break;
                    case SDLK_s:
                        keypad[8] = 0;
                        break;
                    case SDLK_d:
                        keypad[9] = 0;
                        break;
                    case SDLK_f:
                        keypad[0xE] = 0;
                        break;
                    case SDLK_z:
                        keypad[0xA] = 0;
                        break;
                    case SDLK_x:
                        keypad[0] = 0;
                        break;
                    case SDLK_c:
                        keypad[0xB] = 0;
                        break;
                    case SDLK_v:
                        keypad[0xF] = 0;
                        break;
                }
            }
        }

        return quit;
    }

};