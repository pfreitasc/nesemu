#ifndef NES_H
#define NES_H

#include "cpu.h"
#include "cartridge.h"
#include "ppu.h"

typedef struct nes {
    Cpu cpu;
    Ppu ppu;
    Cartridge cartridge;
} Nes;

//function prototypes
void Nes_initPpu(Nes *nes);
void Nes_init(Nes *nes);
void Nes_loadCartridge(Nes *nes, char *filename);

#endif
