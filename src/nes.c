#include "nes.h"

void Nes_initPpu(Nes *nes) {
    //making it so the ppu registers point to the cpu mem location for those registers
    nes->ppu.ppuctrl = &(nes->cpu.mem[2000]);
    nes->ppu.ppumask = &(nes->cpu.mem[2001]);
    nes->ppu.ppustatus = &(nes->cpu.mem[2002]);
    nes->ppu.oamaddr = &(nes->cpu.mem[2003]);
    nes->ppu.oamdata = &(nes->cpu.mem[2004]);
    nes->ppu.ppuscroll = &(nes->cpu.mem[2005]);
    nes->ppu.ppuaddr = &(nes->cpu.mem[2006]);
    nes->ppu.ppudata = &(nes->cpu.mem[2007]);

    *(nes->ppu.ppuctrl) = 0x00;
    *(nes->ppu.ppumask) = 0x00;
    *(nes->ppu.ppustatus) = 0x00;
    *(nes->ppu.oamaddr) = 0x00;
    *(nes->ppu.oamdata) = 0x00;
    *(nes->ppu.ppuscroll) = 0x00;
    *(nes->ppu.ppuaddr) = 0x00;
    *(nes->ppu.data) = 0x00;
}

void Nes_loadCartridge(Nes *nes, char *filename) {
    Cartridge_getData(&(nes->cartridge), filename);
    Cpu_loadRom(&(nes->cpu), nes->cartridge.prg_data);
}

void Nes_init(Nes *nes) {
    Cpu_powerUp(&(nes->cpu));
    Nes_loadCartridge(nes, "../roms/nestest.nes");
    Nes_initPpu(nes);
}