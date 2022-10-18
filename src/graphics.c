#include "graphics.h"

void Graphics_init(Graphics *graphics) {
    int i;

    //clearing viewport
    for (i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
        graphics->game_viewport[i] = 0;
    for (i = 0; i < PATTERNS_WIDTH * PATTERNS_HEIGHT; i++)
        graphics->patterns_viewport[i] = 0;

    graphics->game_window = NULL;
    graphics->game_renderer = NULL;
    graphics->game_texture = NULL;
    graphics->patterns_window = NULL;
    graphics->patterns_renderer = NULL;
    graphics->patterns_texture = NULL;

    //initializing SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return;
    }
    //initializing SDL game screen variables
    graphics->game_window = SDL_CreateWindow("NES", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH*SCREEN_SCALE_FACTOR, SCREEN_HEIGHT*SCREEN_SCALE_FACTOR, SDL_WINDOW_SHOWN);
    if(graphics->game_window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return;
    }
    graphics->game_renderer = SDL_CreateRenderer(graphics->game_window, -1, 0);
    if (graphics->game_renderer == NULL) {
        printf("SDL game_renderer could not be created.\n");
        return;
    }
    graphics->game_texture = SDL_CreateTexture(graphics->game_renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
    if (graphics->game_texture == NULL) {
        printf("SDL game_texture could not be created.\n");
        return;
    }
    //initializing SDL patterns screen variables
    graphics->patterns_window = SDL_CreateWindow("Patterns", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, PATTERNS_WIDTH*SCREEN_SCALE_FACTOR, PATTERNS_HEIGHT*SCREEN_SCALE_FACTOR, SDL_WINDOW_SHOWN);
    if(graphics->patterns_window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return;
    }
    graphics->patterns_renderer = SDL_CreateRenderer(graphics->patterns_window, -1, 0);
    if (graphics->patterns_renderer == NULL) {
        printf("SDL patterns_renderer could not be created.\n");
        return;
    }
    graphics->patterns_texture = SDL_CreateTexture(graphics->patterns_renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_TARGET, PATTERNS_WIDTH, PATTERNS_HEIGHT);
    if (graphics->patterns_texture == NULL) {
        printf("SDL patterns_texture could not be created.\n");
        return;
    }


    //map palette colors to a rgb SDL color
    graphics->palColor[0] = 0x7C7C7C;
    graphics->palColor[1] = 0x0000FC;
    graphics->palColor[2] = 0x0000BC;
    graphics->palColor[3] = 0x4428BC;
    graphics->palColor[4] = 0x940084;
    graphics->palColor[5] = 0xA80020;
    graphics->palColor[6] = 0xA81000;
    graphics->palColor[7] = 0x881400;
    graphics->palColor[8] = 0x503000;
    graphics->palColor[9] = 0x007800;
    graphics->palColor[10] = 0x006800;
    graphics->palColor[11] = 0x005800;
    graphics->palColor[12] = 0x004058;
    graphics->palColor[13] = 0x000000;
    graphics->palColor[14] = 0x000000;
    graphics->palColor[15] = 0x000000;
    graphics->palColor[16] = 0xBCBCBC;
    graphics->palColor[17] = 0x0078F8;
    graphics->palColor[18] = 0x0058F8;
    graphics->palColor[19] = 0x6844FC;
    graphics->palColor[20] = 0xD800CC;
    graphics->palColor[21] = 0xE40058;
    graphics->palColor[22] = 0xF83800;
    graphics->palColor[23] = 0xE45C10;
    graphics->palColor[24] = 0xAC7C00;
    graphics->palColor[25] = 0x00B800;
    graphics->palColor[26] = 0x00A800;
    graphics->palColor[27] = 0x00A844;
    graphics->palColor[28] = 0x008888;
    graphics->palColor[29] = 0x000000;
    graphics->palColor[30] = 0x000000;
    graphics->palColor[31] = 0x000000;
    graphics->palColor[32] = 0xF8F8F8;
    graphics->palColor[33] = 0x3CBCFC;
    graphics->palColor[34] = 0x6888FC;
    graphics->palColor[35] = 0x9878F8;
    graphics->palColor[36] = 0xF878F8;
    graphics->palColor[37] = 0xF85898;
    graphics->palColor[38] = 0xF87858;
    graphics->palColor[39] = 0xFCA044;
    graphics->palColor[40] = 0xF8B800;
    graphics->palColor[41] = 0xB8F818;
    graphics->palColor[42] = 0x58D854;
    graphics->palColor[43] = 0x58F898;
    graphics->palColor[44] = 0x00E8D8;
    graphics->palColor[45] = 0x787878;
    graphics->palColor[46] = 0x000000;
    graphics->palColor[47] = 0x000000;
    graphics->palColor[48] = 0xFCFCFC;
    graphics->palColor[49] = 0xA4E4FC;
    graphics->palColor[50] = 0xB8B8F8;
    graphics->palColor[51] = 0xD8B8F8;
    graphics->palColor[52] = 0xF8B8F8;
    graphics->palColor[53] = 0xF8A4C0;
    graphics->palColor[54] = 0xF0D0B0;
    graphics->palColor[55] = 0xFCE0A8;
    graphics->palColor[56] = 0xF8D878;
    graphics->palColor[57] = 0xD8F878;
    graphics->palColor[58] = 0xB8F8B8;
    graphics->palColor[59] = 0xB8F8D8;
    graphics->palColor[60] = 0x00FCFC;
    graphics->palColor[61] = 0xF8D8F8;
    graphics->palColor[62] = 0x000000;
    graphics->palColor[63] = 0x000000;

    //clear SDL screen
    SDL_SetRenderDrawColor(graphics->game_renderer, 0, 0, 0, 0);
    SDL_RenderClear(graphics->game_renderer);
    SDL_RenderPresent(graphics->game_renderer);
    //clear patterns screen
    SDL_SetRenderDrawColor(graphics->patterns_renderer, 0, 0, 0, 0);
    SDL_RenderClear(graphics->patterns_renderer);
    SDL_RenderPresent(graphics->patterns_renderer);
}

//updates screen with content of viewport vector
void Graphics_drawGame(Graphics *graphics) {
    SDL_UpdateTexture(graphics->game_texture, NULL, graphics->game_viewport, SCREEN_WIDTH * sizeof(unsigned int));
    SDL_RenderClear(graphics->game_renderer);
    SDL_RenderCopy(graphics->game_renderer, graphics->game_texture, NULL, NULL);
    SDL_RenderPresent(graphics->game_renderer);
}

void Graphics_drawPatterns(Graphics *graphics) {
    SDL_UpdateTexture(graphics->patterns_texture, NULL, graphics->patterns_viewport, PATTERNS_WIDTH * sizeof(unsigned int));
    SDL_RenderClear(graphics->patterns_renderer);
    SDL_RenderCopy(graphics->patterns_renderer, graphics->patterns_texture, NULL, NULL);
    SDL_RenderPresent(graphics->patterns_renderer);
}