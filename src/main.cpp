#include "main.h"

#include <iostream>
using namespace std;

int main(int argc, char** argv) {
    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK);
    
    MainWindow = SDL_CreateWindow("My SDL Empty Window",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, 0);
    
    if (MainWindow == NULL) {
        cerr << "Could not create window: " << SDL_GetError() << endl;
        return 1;
    }
    
    while (!quit) {
        SDL_WaitEvent(&event);
        
        switch (event.type) {
            case SDL_QUIT:
                quit = true;
                break;
        }
    }
    
    SDL_DestroyWindow(MainWindow);
    
    return 0;
}
