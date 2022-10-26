#include "nes.h"

void Nes_loadCartridge(Nes *nes, char *filename) {
    Cartridge_getData(&(nes->cartridge), filename);
    Cpu_loadRom(&(nes->cpu), nes->cartridge.prg_data);
    Ppu_loadPatterns(&(nes->cpu.ppu), nes->cartridge.chr_data);
}

void Nes_init(Nes *nes) {
    Cpu_powerUp(&(nes->cpu));
    Nes_loadCartridge(nes, "../roms/DK.nes");
    nes->globalCyclesCounter = 0;
}

void Nes_mainLoop(Nes *nes) {
    while (1) {
        Ppu_tick(&(nes->cpu.ppu));

        if ((nes->globalCyclesCounter % 3) == 0) {
            Cpu_decode(&(nes->cpu));
            Cpu_time(&(nes->cpu));
        }

        nes->globalCyclesCounter += 1;
    }
}