#include <iostream>
#include <fstream>
#include <iomanip>

#include <SDL2/SDL.h>

using namespace std;


int main(int argc, char* argv[]) {
    uint32_t video[64 * 32]{}; // 0x00000000 or 0xFFFFFFFF

    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;

    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        cout << "Video init error\n";
        return 0;
    }

    SDL_CreateWindowAndRenderer(640, 320, 0, &window, &renderer);

    if(window == NULL){
        cout << "Window creation error\n";
        return 0;
    }

    if(renderer == NULL){
        cout << "Renderer creation error\n";
        return 0;
    }
    

    
    // background set to this color
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    SDL_Rect pixel;

    // set color to white
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    bool isRunning = true;
    SDL_Event evt;

    int i = 0;
    while(isRunning){
        while(SDL_PollEvent(&evt) != 0){
            if(evt.type == SDL_QUIT){
                isRunning = false;
            } else if(evt.type == SDL_KEYDOWN){
                switch(evt.key.keysym.sym){
                    case SDLK_1:

                        if(i < 64){
                            i += 1;
                        } else{
                            i = 0;
                        }
                        
                        video[64 + i] = 0xFFFFFFFF;

                        pixel.x = 200;
                        pixel.y = 200;
                        pixel.w = 10;
                        pixel.h = 10;

                        SDL_RenderFillRect(renderer, &pixel);
                        SDL_RenderPresent(renderer);

                        break;
                }
            }
        }

    }

    SDL_DestroyWindow(window);
    SDL_Quit();
   
    return 0;
}