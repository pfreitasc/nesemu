#ifndef PPU_H
#define PPU_H

#include "graphics.h"

#define PALETTE_MEM_START 0x3F00

typedef struct oam {
    unsigned char y;
    unsigned char id;
    unsigned char attribute;
    unsigned char x;
}Oam;

typedef struct ppu {
    //cpu registers
    unsigned char ppuctrl; //$2000 in cpu mem
    unsigned char ppumask; //$2001 in cpu mem
    unsigned char ppustatus; //$2002 in cpu mem
    unsigned char oamaddr; //$2003 in cpu mem
    unsigned char oamdata; //$2004 in cpu mem
    unsigned char ppuscroll; //$2005 in cpu mem
    unsigned short ppuaddr; //$2006 in cpu mem
    unsigned char ppudata; //$2007 in cpu mem
    unsigned char oamdma; //$4014 in cpu mem
    //internal registers
    unsigned short addr_buffer;
    unsigned char read_buffer;
    unsigned char addr_latch;
    unsigned char mirroring;
    unsigned char nmi;
    unsigned char frame_complete;
    short scanline;
    short cycle;
    //background vram register
    unsigned char coarse_x; //tile x, 5 bits
    unsigned char coarse_y; //tile y, 5 bits
    unsigned char nametable_x; //nametable x, 1 bit
    unsigned char nametable_y; // nametable y, 1 bit
    unsigned char fine_x; //pixel x inside tile, 3 bits
    unsigned char fine_y; //pixel y inside tile, 3 bits
    //background tram register
    unsigned char t_coarse_x; //tile x, 5 bits
    unsigned char t_coarse_y; //tile y, 5 bits
    unsigned char t_nametable_x; //nametable x, 1 bit
    unsigned char t_nametable_y; // nametable y, 1 bit
    unsigned char t_fine_y; //pixel y inside tile, 3 bits

    //background rendering registers
    unsigned char bg_next_tile_id; //coordinates of next tile
    unsigned char bg_next_tile_palette; //palette of next tile
    unsigned char bg_next_tile_lsb; //lsb of pattern to load on the tile
    unsigned char bg_next_tile_msb; //msb of pattern to load on the tile
    unsigned short bg_shifter_pattern[2]; 
    unsigned short bg_shifter_palette[2];
    //sprite registers
    Oam oam[64];
    unsigned char *pOam;
    Oam scanline_sprites[8];
    unsigned char sprite_count;
    unsigned char oam_pattern_shift_lo[8];
    unsigned char oam_pattern_shift_hi[8];
    //sprite zero collision flags
    unsigned char spriteZeroHitPossible;
    unsigned char spriteZeroBeingRendered;
    //memory
    unsigned char mem[0x3FFF];  //0000-1FFF: pattern memory
                                //2000-2FFF: nametables
                                //3F00-3F1F: palettes
    //SDL variables struct
    Graphics graphics;
} Ppu;

//function prototypes
void Ppu_init(Ppu *ppu);
unsigned char Ppu_read(Ppu *ppu, unsigned short addr);
void Ppu_write(Ppu *ppu, unsigned char data, unsigned short addr);
void Ppu_draw(Ppu *ppu);
void Ppu_drawPatterns(Ppu *ppu);
void Ppu_loadPatterns(Ppu *ppu, unsigned char *chr_data);
void Ppu_tick(Ppu *ppu);

#endif