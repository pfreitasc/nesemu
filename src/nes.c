#include "nes.h"

void Nes_loadCartridge(Nes *nes, char *filename) {
    Cartridge_getData(&(nes->cartridge), filename);
    Cpu_loadRom(&(nes->cpu), nes->cartridge.prg_data);
    Ppu_loadPatterns(&(nes->cpu.ppu), nes->cartridge.chr_data);
}

void Nes_init(Nes *nes) {
    Cpu_powerUp(&(nes->cpu));
    Nes_loadCartridge(nes, "../roms/smb.nes");
    nes->globalCyclesCounter = 0;
}

void Nes_updateController(Nes *nes) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_KEYDOWN) {
            nes->cpu.controller.pad[0] = 0;
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
        if (event.type == SDL_KEYUP) {
            nes->cpu.controller.pad[0] = 0;
        }
    }
    //printf("\nController 0: %02X\n", nes->cpu.controller.pad[0]);
}

void Nes_mainLoop(Nes *nes) {
    while (1) {
        Ppu_tick(&(nes->cpu.ppu));
        if (nes->globalCyclesCounter % 3 == 0) {
            if (nes->cpu.dma_transfer) {
                //will start dma transfer
                if (nes->cpu.dma_dummy) {
                    //needs to wait for even cycle
                    if ((nes->globalCyclesCounter % 2) == 1) {
                        //cycle is odd
                        nes->cpu.dma_dummy = 0x00;
                    }
                }
                else {
                    //dma begins
                    if ((nes->globalCyclesCounter % 2) == 0) {
                        //fetches data to write in memory
                        nes->cpu.dma_data = nes->cpu.mem[(nes->cpu.dma_page << 8) | nes->cpu.dma_addr];
                    }
                    else {
                        //write to ppu oam on odd clock cycles
                        nes->cpu.ppu.pOam[nes->cpu.dma_addr] = nes->cpu.dma_data;
                        nes->cpu.dma_addr += 1;
                        if (nes->cpu.dma_addr == 0x00) {
                            //page has ended. dma transfer has finished.
                            nes->cpu.dma_transfer = 0x00;
                            nes->cpu.dma_dummy = 0x01;
                        }
                    }
                }
            }
            else {
                //no dma transfer. Cpu can execute normally
                Nes_updateController(nes);
                Cpu_tick(&(nes->cpu));
            }
        }
        nes->globalCyclesCounter += 1;
    }
}