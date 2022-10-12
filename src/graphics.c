#include "graphics.h"

void Graphics_init(Graphics *graphics) {
    int i;

    //clearing viewport
    for (i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
        graphics->viewport[i] = 0;

    graphics->window = NULL;
    graphics->renderer = NULL;
    graphics->texture = NULL;

    //initializing SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return;
    }
    //initializing SDL global variables
    graphics->window = SDL_CreateWindow("NES", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH*SCREEN_SCALE_FACTOR, SCREEN_HEIGHT*SCREEN_SCALE_FACTOR, SDL_WINDOW_SHOWN);
    if(graphics->window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return;
    }
    graphics->renderer = SDL_CreateRenderer(graphics->window, -1, 0);
    if (graphics->renderer == NULL) {
        printf("SDL renderer could not be created.\n");
        return;
    }
    graphics->texture = SDL_CreateTexture(graphics->renderer, SDL_PIXELFORMAT_RGB332, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
    if (graphics->texture == NULL) {
        printf("SDL texture could not be created.\n");
        return;
    }

    //clear SDL screen
    SDL_SetRenderDrawColor(graphics->renderer, 0, 0, 0, 0);
    SDL_RenderClear(graphics->renderer);
    SDL_RenderPresent(graphics->renderer);
}

//updates screen with content of viewport vector
void Graphics_draw(Graphics *graphics) {
    SDL_UpdateTexture(graphics->texture, NULL, graphics->viewport, SCREEN_WIDTH * sizeof(unsigned char));
    SDL_RenderClear(graphics->renderer);
    SDL_RenderCopy(graphics->renderer, graphics->texture, NULL, NULL);
    SDL_RenderPresent(graphics->renderer);
}
