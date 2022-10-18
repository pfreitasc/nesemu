#include "nes.h"


void Nes_loadCartridge(Nes *nes, char *filename) {
    Cartridge_getData(&(nes->cartridge), filename);
    Cpu_loadRom(&(nes->cpu), nes->cartridge.prg_data);
    Ppu_loadPatterns(&(nes->ppu), nes->cartridge.chr_data);
}

void Nes_ppuInit(Nes *nes) {
    (nes->ppu.ppuctrl) = &(nes->cpu.mem[2000]);
    (nes->ppu.ppumask) = &(nes->cpu.mem[2001]);
    (nes->ppu.ppustatus) = &(nes->cpu.mem[2002]);
    (nes->ppu.oamaddr) = &(nes->cpu.mem[2003]);
    (nes->ppu.oamdata) = &(nes->cpu.mem[2004]);
    (nes->ppu.ppuscroll) = &(nes->cpu.mem[2005]);
    (nes->ppu.ppuaddr) = &(nes->cpu.mem[2006]);
    (nes->ppu.ppudata) = &(nes->cpu.mem[2007]);
    (nes->ppu.oamdma) = &(nes->cpu.mem[4014]);

    Ppu_init(&(nes->ppu));
}

void Nes_init(Nes *nes) {
    Cpu_powerUp(&(nes->cpu));
    Nes_loadCartridge(nes, "../roms/DK.nes");
    Nes_ppuInit(nes);
    nes->globalCyclesCounter = 7;
}

void Nes_mainLoop(Nes *nes) {
    while (1) {
        Cpu_decode(&(nes->cpu));
        if (nes->ppu.nmi) {
            nes->ppu.nmi = 0;
            Cpu_nmi(&(nes->cpu));
        }


        nes->globalCyclesCounter += Cpu_tick(&(nes->cpu));
        #ifdef DEBUG
        printf(" CYC:%u\n", nes->globalCyclesCounter);
        #endif

        Ppu_draw(&(nes->ppu));
    }
}