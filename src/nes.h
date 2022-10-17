#ifndef NES_H
#define NES_H

#include "cpu.h"
#include "ppu.h"
#include "cartridge.h"

typedef struct {
    Cpu cpu;
    Ppu ppu;
    Cartridge cartridge;
    unsigned short globalCyclesCounter; //number elapsed cycles
} Nes;

//function prototypes
void Nes_init(Nes *nes);
void Nes_ppuInit(Nes *nes);
void Nes_loadCartridge(Nes *nes, char *filename);
void Nes_mainLoop(Nes *nes);

#endif
