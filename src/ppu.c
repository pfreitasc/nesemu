#include "ppu.h"
#include "graphics.h"

//bus must be initialized before
void Ppu_init(Ppu *ppu) {
    *(ppu->ppuctrl) = 0;
    *(ppu->ppumask) = 0;
    *(ppu->ppustatus) = 0;
    *(ppu->oamaddr) = 0;
    *(ppu->ppuscroll) = 0;
    *(ppu->ppudata) = 0;

    Graphics_init(&(ppu->graphics));
    int i;
    for (i = 0; i < SCREEN_HEIGHT*SCREEN_WIDTH; i++)
        ppu->graphics.viewport[i] = i;
    Ppu_draw(ppu);
}

void Ppu_draw(Ppu *ppu) {
    Graphics_draw(&(ppu->graphics));
}