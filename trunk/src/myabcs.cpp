#include "myabcs.h"
#include "window.h"

#include <iostream>
using namespace std;

bool quit = false;

int main(int argc, char** argv) {
    loadMainWindow();
    
    SDL_Event event;
    
    while (!quit) {
        SDL_WaitEvent(&event);
        
        switch (event.type) {
            case SDL_QUIT:
                quit = true;
                break;
        }
    }
    
    return closeMainWindow();
}
