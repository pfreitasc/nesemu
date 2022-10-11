#include "nes.h"


void Nes_loadCartridge(Nes *nes, char *filename) {
    Cartridge_getData(&(nes->cartridge), filename);
    Cpu_loadRom(&(nes->cpu), nes->cartridge.prg_data);
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

    Ppu_init(&(nes->ppu));
}

void Nes_init(Nes *nes) {
    Cpu_powerUp(&(nes->cpu));
    Nes_loadCartridge(nes, "../roms/nestest.nes");
    Nes_ppuInit(nes);
}