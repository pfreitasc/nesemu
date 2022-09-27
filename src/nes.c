#include "nes.h"


void Nes_loadCartridge(Nes *nes, char *filename) {
    Cartridge_getData(nes->cartridge, filename);
    Cpu_loadRom(nes->cpu, nes->cartridge.prg_data);
}

void Nes_init(Nes *nes) {
    Cpu_powerUp(nes->cpu);
    Nes_loadCartridge(nes, "../roms/nestest.nes");
}