#ifndef DISPLAY_H
#define DISPLAY_H
#include <SDL2/SDL.h>

SDL_Window *initDisplay(int width, int height);
SDL_Renderer *initRender(SDL_Window *wind);

#endif
