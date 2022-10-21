#include "ppu.h"
#include "graphics.h"

unsigned char Ppu_getIncrement(Ppu *ppu) {
    //I is 1: increment 32
    if ((*(ppu->ppuctrl) & 0x04) & 0x04)
        return 32;
    //I is 0: increment 1
    else
        return 1;
}

unsigned char Ppu_readStatus(Ppu *ppu) {
    ppu->addr_latch = 0;
    *(ppu->ppustatus) &= 0x7F;
    *(ppu->ppustatus) |= (ppu->read_buffer & 0x1F);
    return *(ppu->ppustatus);
}

unsigned char Ppu_readData(Ppu *ppu) {
    unsigned char data;
    data = ppu->read_buffer;
    ppu->read_buffer = ppu->mem[*(ppu->ppuaddr)];
    //if reading palette data, doesn't write to buffer before returning value
    if (*(ppu->ppuaddr) >= 0x3F00)
        data = ppu->read_buffer;
    
    //incrementing addr
    *(ppu->ppuaddr) += Ppu_getIncrement(ppu);
    return data;
}

void Ppu_writeCtrl(Ppu *ppu, unsigned char data) {
    *(ppu->ppuctrl) = data;
    //nametable x and y on the loopy reg
    ppu->nametable_x = (data & 0x01);
    ppu->nametable_y = (data & 0x02);
}

void Ppu_writeScroll(Ppu *ppu, unsigned char data) {
    if (ppu->addr_latch == 0) {
        ppu->fine_x = data & 0x07;
        ppu->coarse_x = data >> 3;
        *(ppu->ppuscroll) = data;
        ppu->addr_latch = 1;
    }
    else {
        ppu->fine_y = data & 0x07;
        ppu->coarse_y = data >> 3;
        *(ppu->ppuscroll) = data;
        ppu->addr_latch = 0;        
    }
}

void Ppu_writeAddr(Ppu *ppu, unsigned char data) {
    if (ppu->addr_latch == 0) {
        ppu->addr_buffer = (unsigned short) data;
        ppu->addr_buffer <<= 8;
        ppu->addr_latch = 1;
    }
    else {
        ppu->addr_buffer |= (unsigned short) data;
        *(ppu->ppuaddr) = ppu->addr_buffer;
        ppu->addr_latch = 0;
    }
}

void Ppu_writeData(Ppu *ppu) {
    ppu->mem[(*(ppu->ppuaddr))] = *(ppu->ppudata);
    *(ppu->ppuaddr) += Ppu_getIncrement(ppu);
}


void Ppu_updateViewport(Ppu *ppu, unsigned char pal) {
    int i;
    //loading palette for testing
    ppu->mem[PALETTE_MEM_START] = 0x3F;
    ppu->mem[PALETTE_MEM_START + pal*4 + 1] = 0x07;
    ppu->mem[PALETTE_MEM_START + pal*4 + 2] = 0x17;
    ppu->mem[PALETTE_MEM_START + pal*4 + 3] = 0x21;
    for (i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        if (ppu->patterns_matrix[i] == 0)
            ppu->graphics.patterns_viewport[i] = ppu->graphics.palColor[ppu->mem[PALETTE_MEM_START]];
        else
            ppu->graphics.patterns_viewport[i] = ppu->graphics.palColor[ppu->mem[PALETTE_MEM_START + pal*4 + ppu->patterns_matrix[i]]];
    }
}

void Ppu_draw(Ppu *ppu) {
    Graphics_drawPatterns(&(ppu->graphics));
    Graphics_drawGame(&(ppu->graphics));
}

void Ppu_loadPatterns(Ppu *ppu, unsigned char *chr_data) {
    //mapper 0
    int i;
    for (i = 0; i < 0x1FFF; i++) {
        ppu->mem[0 + i] = chr_data[i];
    }
}

void Ppu_drawPatterns(Ppu *ppu) {
    int table, tile_row, tile_column, pixel_row, pixel_column, matrix_addr;
    unsigned short addr_0, addr_1;
    unsigned char bit_0, bit_1, bit;
    //per table
    for(table = 0; table < 2; table++) {
        //per tile
        for (tile_row = 0; tile_row < 16; tile_row++) {
            //per tile column
            for (tile_column = 0; tile_column < 16; tile_column++){
                //per pixel row
                for (pixel_row = 0; pixel_row < 8; pixel_row++) {
                    //bit plane 0
                    addr_0 = 0 | (table << 12) | (tile_row << 8) | (tile_column << 4) | (pixel_row);
                    //bit plane 1
                    addr_1 = addr_0 | (1 << 3);
                    //per pixel column
                    for (pixel_column = 0; pixel_column < 8; pixel_column++) {
                        matrix_addr = (pixel_column) + (pixel_row*PATTERNS_WIDTH) + (tile_column*8) + (tile_row*8*PATTERNS_WIDTH) + (table*16*8);
                        bit_0 = ((ppu->mem[addr_0] & (0x80 >> pixel_column)) >> (7 - pixel_column));
                        bit_1 = ((ppu->mem[addr_1] & (0x80 >> pixel_column)) >> (7 - pixel_column));
                        bit = bit_0 + bit_1;
                        ppu->patterns_matrix[matrix_addr] = bit;
                    }
                }
            }
        }
    }
    Ppu_updateViewport(ppu, 2);
    Ppu_draw(ppu);
}

//bus must be initialized before
void Ppu_init(Ppu *ppu) {
    *(ppu->ppuctrl) = 0;
    *(ppu->ppumask) = 0;
    *(ppu->ppustatus) = 0;
    *(ppu->oamaddr) = 0;
    *(ppu->ppuscroll) = 0;
    *(ppu->ppudata) = 0;

    Graphics_init(&(ppu->graphics));
    //Ppu_draw(ppu);
    Ppu_drawPatterns(ppu);
}

Ppu_tick(Ppu *ppu) {
    //on a visible scanline (or pre-render)
    if ((ppu->scanline >= -1) && (ppu->scanline < 240)) {
        if ((ppu->scanline == 0) && (ppu->cycle == 0)) {
            //cycle 0 is skipped on scanline 0
            ppu->cycle = 1;
        }
        //on the pre-render scanline
        if ((ppu->scanline == -1) && (ppu->cycle == 1)) {
            //entering a new frame, clear:
            //VBlank, Sprite 0, Overflow
            *(ppu->ppustatus) &= 0x7F;
        }
        if (((ppu->cycle >= 2) && (ppu->cycle < 258)) || ((ppu->cycle >= 321) && (ppu->cycle < 338))) {
            //if BG is enabled in ppumask
            if ((*(ppu->ppumask) & 0x08) == 0x08) {
                //shift BG shifters
                ppu->bg_shifter_pattern[0] <<= 1;
                ppu->bg_shifter_pattern[1] <<= 1;
                ppu->bg_shifter_attrib[0] <<= 1;
                ppu->bg_shifter_attrib[1] <<= 1;
            }
            switch ((ppu->cycle - 1) % 8) {
                case 0: //NT byte (cycle = 9 (0?))
                    //load the current BG tile pattern into the shifter
                    //fetch next tile pattern
                    ppu->bg_next_tile_id = ppu->mem[0x2000 | (*(ppu->ppuaddr) & 0x0FFF)];
                case 2: //AT byte (cycle = 3)
                case 4: //Low BG tile byte (cycle = 5)
                case 6: //High BG tile byte (cycle = 7)
                case 7: //finished 8 pixels, increment scroll X
            }
        }
    }

    //just exited visible scanline
    if ((ppu->scanline == 241) && (ppu->cycle == 1)) {
        //setting VBlank
        *(ppu->ppustatus) |= 0x80;
        //emit nmi if ppucontrol nmi is set
        if ((*(ppu->ppuctrl) & 0x80) == (0x80))
            ppu->nmi = 0x01;
    }

    //incrementing scanline and/or cycle
    ppu->cycle += 1;
    if (ppu->cycle >= 341) {
        ppu->cycle = 0;
        ppu->scanline += 1;
        if (ppu->scanline >= 261) {
            ppu->scanline = -1;
            ppu->frame_complete = 0x01;
        }
    }
}