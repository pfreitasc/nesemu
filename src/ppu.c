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

unsigned char Ppu_read(Ppu *ppu, unsigned short addr) {
    unsigned char data;
    switch (addr) {
    case 0x2002:
        data = Ppu_readStatus(ppu);
        break;
    case 0x2007:
        data = Ppu_readData(ppu);
    }
    return data;
}

void Ppu_writeCtrl(Ppu *ppu, unsigned char data) {
    *(ppu->ppuctrl) = data;
    //nametable x and y on the loopy reg
    ppu->t_nametable_x = (data & 0x01);
    ppu->t_nametable_y = (data & 0x02);
}

void Ppu_writeScroll(Ppu *ppu, unsigned char data) {
    if (ppu->addr_latch == 0) {
        ppu->fine_x = data & 0x07;
        ppu->t_coarse_x = data >> 3;
        *(ppu->ppuscroll) = data;
        ppu->addr_latch = 1;
    }
    else {
        ppu->t_fine_y = data & 0x07;
        ppu->t_coarse_y = data >> 3;
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
        //what was stored in the bufffer gets stored in vram
        *(ppu->ppuaddr) = ppu->addr_buffer;
        //tram is stored in vram
        ppu->coarse_x = ppu->t_coarse_x;
        ppu->coarse_y = ppu->t_coarse_y;
        ppu->nametable_x = ppu->t_nametable_x;
        ppu->nametable_y = ppu->t_nametable_y;
        ppu->fine_y = ppu->t_fine_y;
        ppu->addr_latch = 0;
    }
}

void Ppu_writeData(Ppu *ppu) {
    ppu->mem[(*(ppu->ppuaddr))] = *(ppu->ppudata);
    *(ppu->ppuaddr) += Ppu_getIncrement(ppu);
}

void Ppu_write(Ppu *ppu, unsigned char data, unsigned short addr) {
    switch (addr) {
    case 0x2000:
        Ppu_writeCtrl(ppu, data);
        break;
    case 0x2005:
        Ppu_writeScroll(ppu, data);
        break;
    case 0x2006:
        Ppu_writeAddr(ppu, data);
        break;
    case 0x2007:
        Ppu_writeData(ppu);
        break;
    }
}

unsigned int Ppu_getPixelVal(Ppu *ppu, unsigned char pixel, unsigned char pal) {
    unsigned int colored_pixel;
    if (pixel == 0)
        colored_pixel = ppu->graphics.palColor[ppu->mem[PALETTE_MEM_START]];
    else
        colored_pixel = ppu->graphics.palColor[ppu->mem[PALETTE_MEM_START] + pal*4 + pixel - 1];
    return colored_pixel;
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
                        ppu->graphics.patterns_viewport[matrix_addr] = Ppu_getPixelVal(ppu, bit, 0);
                    }
                }
            }
        }
    }
    Ppu_draw(ppu);
}

//bus must be initialized before
void Ppu_init(Ppu *ppu) {
    *(ppu->ppuctrl) = 0;
    *(ppu->ppumask) = 0;
    *(ppu->ppustatus) = 0;
    *(ppu->oamaddr) = 0;
    *(ppu->ppuscroll) = 0xA0;
    *(ppu->ppudata) = 0;
    ppu->t_coarse_x = 0;
    ppu->t_coarse_y = 0;
    ppu->t_nametable_x = 0;
    ppu->t_nametable_y = 0;
    ppu->t_fine_y = 0;
    ppu->coarse_x = 0;
    ppu->coarse_y = 0;
    ppu->nametable_x = 0;
    ppu->nametable_y = 0;
    ppu->fine_y = 0;
    ppu->fine_x = 0;
    printf("PPU PPUSCROLL: %02X\n", *(ppu->ppuscroll));

    Graphics_init(&(ppu->graphics));
}

unsigned short Ppu_getVram(Ppu *ppu) {
    unsigned short vram = (((unsigned short) ppu->fine_y << 12) | ((unsigned short) ppu->nametable_y << 11) | ((unsigned short) ppu->nametable_x << 10) | ((unsigned short) ppu->coarse_y << 5) | ((unsigned short) ppu->coarse_x));
    return vram;
}

 void Ppu_tick(Ppu *ppu) {
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
                ppu->bg_shifter_palette[0] <<= 1;
                ppu->bg_shifter_palette[1] <<= 1;
            }

            switch ((ppu->cycle - 1) % 8) {
                case 0: //NT byte (cycle = 9 (0?))
                    //load the next BG tile pattern into the shifters
                    ppu->bg_shifter_pattern[0] = (ppu->bg_shifter_pattern[0] & 0xFF00) | (ppu->bg_next_tile_lsb);
                    ppu->bg_shifter_pattern[1] = (ppu->bg_shifter_pattern[1] & 0xFF00) | (ppu->bg_next_tile_msb);
                    //load the next BG tile palette info to the shifters
                    ppu->bg_shifter_palette[0] &= 0xFF00;
                    ppu->bg_shifter_palette[1] &= 0xFF00;
                    if (ppu->bg_next_tile_palette & 0x01)
                        ppu->bg_shifter_palette[0] |= 0xFF;
                    if (ppu->bg_next_tile_palette & 0x02)
                        ppu->bg_shifter_palette[1] |= 0xFF;
                    //fetch next tile pattern
                    ppu->bg_next_tile_id = ppu->mem[0x2000 | (Ppu_getVram(ppu) & 0x0FFF)];
                    break;
                case 2: //AT byte (cycle = 3)
                    ppu->bg_next_tile_palette = ppu->mem[0x23C0 | ((unsigned short) ppu->nametable_y << 11) | (ppu->nametable_x << 10) | ((ppu->coarse_y >> 2) << 3) | (ppu->coarse_x >> 2)];
                    //choose which one of the 4 2x2 tile sets
                    if (ppu->coarse_y & 0x02)
                        ppu->bg_next_tile_palette >>= 4;
                    if (ppu->coarse_x & 0x02)
                        ppu->bg_next_tile_palette >>= 2;
                    ppu->bg_next_tile_palette &= 0x03;
                    break;
                case 4: //Low BG tile byte (cycle = 5)
                    ppu->bg_next_tile_lsb = ppu->mem[((*(ppu->ppuctrl) & 0x10) << 3) + ((unsigned short) ppu->bg_next_tile_id << 4) + (ppu->fine_y)];
                case 6: //High BG tile byte (cycle = 7)
                    ppu->bg_next_tile_lsb = ppu->mem[((*(ppu->ppuctrl) & 0x10) << 3) + ((unsigned short) ppu->bg_next_tile_id << 4) + (ppu->fine_y) + 8];
                case 7: //finished 8 pixels, increment scroll X
                    if ((*(ppu->ppumask) & 0x08) | ((*ppu->ppumask) & 0x10)) {
                        if (ppu->coarse_x == 31) {
                            ppu->coarse_x = 0;
                            //have exited nametable, onto the next one
                            ppu->nametable_x ^= 0x01;
                        }
                        else {
                            //still on current nametable
                            ppu->coarse_x += 1;
                        }
                    }
                    break;
            }
            if (ppu->cycle == 256) {
                //if rendering is on
                if ((*(ppu->ppumask) & 0x08) | ((*ppu->ppumask) & 0x10)) {
                    //finished a whole line, got to increment y
                    if (ppu->fine_y < 7)
                        ppu->fine_y += 1;
                    else {
                        ppu->fine_y = 0;
                        if (ppu->coarse_y == 29) {
                            ppu->coarse_y = 0;
                            ppu->nametable_y ^= 0x01;
                        }
                        else if (ppu->coarse_y == 31) {
                            ppu->coarse_y = 0;
                        }
                        else {
                            ppu->coarse_y += 1;
                        }
                    }
                }
            }
            if (ppu->cycle == 257) {
                //load the next BG tile palette info to the shifters
                ppu->bg_shifter_palette[0] &= 0xFF00;
                ppu->bg_shifter_palette[1] &= 0xFF00;
                if (ppu->bg_next_tile_palette & 0x01)
                    ppu->bg_shifter_palette[0] |= 0xFF;
                if (ppu->bg_next_tile_palette & 0x02)
                    ppu->bg_shifter_palette[1] |= 0xFF;
                //whole line done, resetting x to scroll position (in tram)
                if ((*(ppu->ppumask) & 0x08) | ((*ppu->ppumask) & 0x10)) {
                    ppu->nametable_x = ppu->t_nametable_x;
                    ppu->coarse_x = ppu->t_coarse_x;
                }
            }
            if ((ppu->cycle == 338) || (ppu->cycle == 340)) {
                //superfluous reads
                ppu->bg_next_tile_id = ppu->mem[(0x2000 | (Ppu_getVram(ppu) & 0x0FFF))];
            }
            if ((ppu->scanline == -1) && (ppu->cycle >= 280) && (ppu->cycle < 305)) {
                //if rendering is enabled
                if ((*(ppu->ppumask) & 0x08) | ((*ppu->ppumask) & 0x10)) {
                    //finished frame, reset y coordinate each tick
                    ppu->fine_y = ppu->t_fine_y;
                    ppu->nametable_y = ppu->t_nametable_y;
                    ppu->coarse_y = ppu->t_coarse_y;
                }
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

    //composing screen
    unsigned char bg_pixel = 0x00;
    unsigned char bg_palette = 0x00;

    //if rendering is on
    if ((*(ppu->ppumask) & 0x08) | ((*ppu->ppumask) & 0x10)) {
        unsigned short bit_mux = 0x8000 >> ppu->fine_x;
        unsigned char p0_pixel = (ppu->bg_shifter_pattern[0] & bit_mux) > 0;
        unsigned char p1_pixel = (ppu->bg_shifter_pattern[1] & bit_mux) > 0;
        bg_pixel = (p1_pixel << 1) | p0_pixel;

        unsigned char bg_pal0 = (ppu->bg_shifter_palette[0] & bit_mux) > 0;
        unsigned char bg_pal1 = (ppu->bg_shifter_palette[1] & bit_mux) > 0;
        bg_palette = (bg_pal1 << 1) | bg_pal0;
    }

    ppu->graphics.game_viewport[(ppu->cycle - 1) + (SCREEN_WIDTH * ppu->scanline)] = Ppu_getPixelVal(ppu, bg_pixel, bg_palette);

    //incrementing scanline and/or cycle
    ppu->cycle += 1;
    if (ppu->cycle >= 341) {
        ppu->cycle = 0;
        ppu->scanline += 1;
        if (ppu->scanline >= 261) {
            ppu->scanline = -1;
            ppu->frame_complete = 0x01;
            Ppu_draw(ppu);
        }
    }
}