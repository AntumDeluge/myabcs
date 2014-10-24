#ifndef MYABCS_WINDOW
#define MYABCS_WINDOW

#include <SDL2/SDL.h>

extern SDL_Window* MainWindow;
extern SDL_Renderer* renderer;

int loadMainWindow();

int closeMainWindow();

#endif /* MYABCS_WINDOW */
