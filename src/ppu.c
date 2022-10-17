#include "ppu.h"
#include "graphics.h"

void Ppu_draw(Ppu *ppu) {
    Graphics_drawPatterns(&(ppu->graphics));
}

void Ppu_loadPatterns(Ppu *ppu, unsigned char *chr_data) {
    //mapper 0
    int i;
    for (i = 0; i < 0x1FFF; i++) {
        ppu->mem[0 + i] = chr_data[i];
    }
}

void Ppu_drawPatterns(Ppu *ppu) {
    int table, tile_row, tile_column, pixel_row, pixel_column, viewport_addr;
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
                        viewport_addr = (pixel_column) + (pixel_row*PATTERNS_WIDTH) + (tile_column*8) + (tile_row*8*PATTERNS_WIDTH) + (table*16*8);
                        bit_0 = ((ppu->mem[addr_0] & (0x80 >> pixel_column)) >> (7 - pixel_column));
                        bit_1 = ((ppu->mem[addr_1] & (0x80 >> pixel_column)) >> (7 - pixel_column));
                        bit = bit_0 + bit_1;
                        ppu->graphics.patterns_viewport[viewport_addr] = bit;
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
    *(ppu->ppuscroll) = 0;
    *(ppu->ppudata) = 0;

    Graphics_init(&(ppu->graphics));
    //Ppu_draw(ppu);
    Ppu_drawPatterns(ppu);
}

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