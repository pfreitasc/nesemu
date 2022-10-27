#include "nes.h"

void Nes_loadCartridge(Nes *nes, char *filename) {
    Cartridge_getData(&(nes->cartridge), filename);
    Cpu_loadRom(&(nes->cpu), nes->cartridge.prg_data);
    Ppu_loadPatterns(&(nes->cpu.ppu), nes->cartridge.chr_data);
}

void Nes_init(Nes *nes) {
    Cpu_powerUp(&(nes->cpu));
    Nes_loadCartridge(nes, "../roms/nestest.nes");
    nes->globalCyclesCounter = 0;
}

void Nes_updateController(Nes *nes) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        nes->cpu.controller.pad[0] = 0;
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_x)
                    nes->cpu.controller.pad[0] |= 0x80;
            if (event.key.keysym.sym == SDLK_z)
                    nes->cpu.controller.pad[0] |= 0x40;
            if (event.key.keysym.sym == SDLK_a)
                    nes->cpu.controller.pad[0] |= 0x20;
            if (event.key.keysym.sym == SDLK_s)
                    nes->cpu.controller.pad[0] |= 0x10;
            if (event.key.keysym.sym == SDLK_UP)
                    nes->cpu.controller.pad[0] |= 0x08;
            if (event.key.keysym.sym == SDLK_DOWN)
                    nes->cpu.controller.pad[0] |= 0x04;
            if (event.key.keysym.sym == SDLK_LEFT)
                    nes->cpu.controller.pad[0] |= 0x02;
            if (event.key.keysym.sym == SDLK_RIGHT)
                    nes->cpu.controller.pad[0] |= 0x01;
            }
        }
    }
    printf("\nController 0: %02X\n", nes->cpu.controller.pad[0]);
}

void Nes_mainLoop(Nes *nes) {
    while (1) {
        Nes_updateController(nes);
        Cpu_decode(&(nes->cpu));
        unsigned char cpu_clocks = Cpu_time(&(nes->cpu));
        unsigned char i;
        for (i = 0; i < cpu_clocks * 3; i++) {
            Ppu_tick(&(nes->cpu.ppu));
        }
        nes->globalCyclesCounter += 1;
    }
}