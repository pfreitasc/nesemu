#include "ppu.h"
#include "graphics.h"


void Ppu_draw(Ppu *ppu) {
    Graphics_draw(&(ppu->graphics));
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
                        viewport_addr = (pixel_column) + (pixel_row*SCREEN_WIDTH) + (tile_column*8) + (tile_row*8*SCREEN_WIDTH) + (table*16*8);
                        bit_0 = ((ppu->mem[addr_0] & (0x80 >> pixel_column)) >> (7 - pixel_column));
                        bit_1 = ((ppu->mem[addr_1] & (0x80 >> pixel_column)) >> (7 - pixel_column));
                        bit = bit_0 + bit_1;
                        ppu->graphics.viewport[viewport_addr] = bit;
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
    int i;
    for (i = 0; i < SCREEN_HEIGHT*SCREEN_WIDTH; i++)
        ppu->graphics.viewport[i] = 0;
    //Ppu_draw(ppu);
    Ppu_drawPatterns(ppu);
}

unsigned char Ppu_readStatus(Ppu *ppu) {
    ppu->write_toggle = 0;
    *(ppu->ppustatus) &= 0x7F;
    *(ppu->ppustatus) |= (ppu->stale_data & 0x1F);
    return *(ppu->ppustatus);
}

unsigned char Ppu_readData(Ppu *ppu) {
    
}