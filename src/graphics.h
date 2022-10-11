#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>
#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 240
#define SCREEN_SCALE_FACTOR 2


typedef struct graphics {
    unsigned char viewport[SCREEN_WIDTH*SCREEN_HEIGHT];
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
} Graphics;

//function prototypes
void Graphics_init(Graphics *graphics);
void Graphics_draw(Graphics *graphics);

#endif