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
    unsigned char *oamdma; //$4014 in cpu mem
    //internal registers
    unsigned short addr_write;
    unsigned short addr_buffer;
    unsigned char read_buffer;
    unsigned char x;
    unsigned char addr_latch;
    unsigned char mirroring;
    unsigned char nmi;
    //background registers
    unsigned char coarse_x; //tile x, 5 bits
    unsigned char coarse_y; //tile y, 5 bits
    unsigned char nametable_x; //nametable x, 1 bit
    unsigned char nametable_y; // nametable y, 1 bit
    unsigned char fine_x; //pixel x inside tile, 3 bits
    unsigned char fine_y; //pixel y inside tile, 3 bits
    unsigned short bg_pattern_shift[2];
    unsigned char bg_palette_shift[2];
    //sprite registers
    unsigned char oam1[64];
    unsigned char oam2[8];
    unsigned char oam_pattern_shift[16];
    unsigned char oam_latch[8];
    unsigned char oam_counter[8];
    //memory
    unsigned char mem[0x3FFF];  //0000-1FFF: pattern memory
                                //2000-2FFF: nametables
                                //3F00-3F1F: palettes
    //SDL variables struct
    Graphics graphics;
} Ppu;

//function prototypes
void Ppu_init(Ppu *ppu);
void Ppu_draw(Ppu *ppu);
void Ppu_loadPatterns(Ppu *ppu, unsigned char *chr_data);

#endif