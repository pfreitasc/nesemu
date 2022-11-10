#ifndef NES_H
#define NES_H

#include "cpu.h"
#include "cartridge.h"
#define DEBUG 1

typedef struct {
    Cpu cpu;
    Cartridge cartridge;
    unsigned short globalCyclesCounter; //number elapsed cycles
} Nes;

//function prototypes
void Nes_init(Nes *nes);
void Nes_loadCartridge(Nes *nes, char *filename);
void Nes_mainLoop(Nes *nes);

#endif
