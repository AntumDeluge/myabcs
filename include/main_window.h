#ifndef MYABCS_MAIN_WINDOW
#define MYABCS_MAIN_WINDOW

#include <SDL2/SDL.h>

extern SDL_Window* MainWindow;
extern SDL_Renderer* renderer;

int loadMainWindow();

int closeMainWindow();

#endif /* MYABCS_MAIN_WINDOW */
