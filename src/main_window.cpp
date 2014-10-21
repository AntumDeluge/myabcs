#include "main_window.h"

#include <iostream>
using namespace std;

SDL_Window* MainWindow;
SDL_Renderer* renderer;

int loadMainWindow() {
    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK);
    
    MainWindow = SDL_CreateWindow("My SDL Empty Window",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, 0);
    if (MainWindow == NULL) {
        cerr << "Could not create SDL_Window: " << SDL_GetError() << endl;
        return 1;
    }
    
    renderer = SDL_CreateRenderer(MainWindow, -1, 0);
    if (renderer == NULL) {
        cerr << "Could not create SDL_Renderer: " << SDL_GetError() << endl;
        return 1;
    }
    
    return 0;
}

int closeMainWindow() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(MainWindow);
    
    return 0;
}
