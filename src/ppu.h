#ifndef PPU_H
#define PPU_H

#include "graphics.h"

typedef struct ppu {
    //cpu registers
    unsigned char *ppuctrl; //$2000 in cpu mem
    unsigned char *ppumask; //$2001 in cpu mem
    unsigned char *ppustatus; //$2002 in cpu mem
    unsigned char *oamaddr; //$2003 in cpu mem
    unsigned char *oamdata; //$2004 in cpu mem
    unsigned char *ppuscroll; //$2005 in cpu mem
    unsigned char *ppuaddr; //$2006 in cpu mem
    unsigned char *ppudata; //$2007 in cpu mem
    //internal registers
    unsigned short addr_write;
    unsigned short addr_buffer;
    unsigned char x;
    unsigned char write_toggle;
    //memory
    unsigned char mem[0x3FFF];  //0000-1FFF: pattern memory
                                //2000-2FFF: nametables
                                //3F00-3F1F: palettes
    //SDL variables struct
    Graphics graphics;
} Ppu;

//function prototypes
void Ppu_init(Ppu *ppu);
void Ppu_writeAddr(Ppu *ppu);
void Ppu_draw(Ppu *ppu);



#endif