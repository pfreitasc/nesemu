#ifndef NES_H
#define NES_H

#include "cpu.h"
#include "cartridge.h"

typedef struct {
    Cpu cpu;
    Cartridge cartridge;
} Nes;

//function prototypes
Nes_init(Nes *nes);
Nes_loadCartridge(Nes *nes);

#endif
