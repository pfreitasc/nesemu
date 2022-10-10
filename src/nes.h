#ifndef NES_H
#define NES_H

#include "cpu.h"
#include "ppu.h"
#include "cartridge.h"

typedef struct {
    Cpu cpu;
    Ppu ppu;
    Cartridge cartridge;
} Nes;

//function prototypes
void Nes_init(Nes *nes);
void Nes_ppuBusInit(Nes *nes);
void Nes_loadCartridge(Nes *nes, char *filename);

#endif
