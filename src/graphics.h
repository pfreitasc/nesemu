#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>
#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 240
#define PATTERNS_WIDTH 256
#define PATTERNS_HEIGHT 240
#define SCREEN_SCALE_FACTOR 2

//palette color lookup

typedef struct graphics {
    unsigned char game_viewport[SCREEN_WIDTH*SCREEN_HEIGHT];
    unsigned char patterns_viewport[PATTERNS_WIDTH*PATTERNS_HEIGHT];
    unsigned int palColor[0x3F];
    SDL_Window *game_window;
    SDL_Renderer *game_renderer;
    SDL_Texture *game_texture;
    SDL_Window *patterns_window;
    SDL_Renderer *patterns_renderer;
    SDL_Texture *patterns_texture;
} Graphics;

//function prototypes
void Graphics_init(Graphics *graphics);
void Graphics_drawPatterns(Graphics *graphics);
void Graphics_drawGame(Graphics *graphics);

#endif