#include "ppu.h"
#include "graphics.h"

unsigned char Ppu_getIncrement(Ppu *ppu) {
    //I is 1: increment 32
    if ((ppu->ppuctrl & 0x04) & 0x04)
        return 32;
    //I is 0: increment 1
    else
        return 1;
}

unsigned short Ppu_getVram(Ppu *ppu) {
    unsigned short vram = (((unsigned short) ppu->fine_y << 12) | ((unsigned short) ppu->nametable_y << 11) | ((unsigned short) ppu->nametable_x << 10) | ((unsigned short) ppu->coarse_y << 5) | ((unsigned short) ppu->coarse_x));
    return vram;
}

unsigned char Ppu_read(Ppu *ppu, unsigned short addr) {
    unsigned char data;
    switch (addr) {
        case 0x2002: //status
            ppu->addr_latch = 0;
            ppu->ppustatus &= 0xE0;
            ppu->ppustatus |= (ppu->read_buffer & 0x1F);
            data = ppu->ppustatus;
            ppu->ppustatus &= 0x7F;
            break;
        case 0x2004: //oamdata
            data = ppu->pOam[ppu->oamaddr];
            ppu->oamdata = data;
            break;
        case 0x2007: //ppudata
            data = ppu->read_buffer;
            ppu->read_buffer = ppu->mem[ppu->ppuaddr];
            //if reading palette data, doesn't write to buffer before returning value
            if (ppu->ppuaddr >= 0x3F00)
                data = ppu->read_buffer;
            //incrementing addr
            ppu->ppuaddr += Ppu_getIncrement(ppu);
        case 0x4014: //oam
            break;
        default:
            break;
    }
    #ifdef DEBUG
    printf("\n-- PPU read --\nreg: %04X\nppuaddr: %04X\ndata: %02X\n", addr, ppu->ppuaddr, data);
    #endif
    return data;
}

void Ppu_write(Ppu *ppu, unsigned char data, unsigned short addr) {
    #ifdef DEBUG
    printf("\n-- PPU write --\nreg: %04X\nppuaddr: %04X\ndata: %02X\n", addr, ppu->ppuaddr, data);
    #endif
    switch (addr) {
    case 0x2000:
        ppu->ppuctrl = data;
        //nametable x and y on the loopy reg
        ppu->t_nametable_x = (data & 0x01);
        ppu->t_nametable_y = (data & 0x02);
        ppu->t_nametable_y >>= 1;
        break;
    case 0x2001:
        ppu->ppumask = data;
        break;
    case 0x2003:
        ppu->oamaddr = data;
        break;
    case 0x2004:
        ppu->pOam[ppu->oamaddr] = data;
        break;
    case 0x2005:
        if (ppu->addr_latch == 0) {
            ppu->fine_x = data & 0x07;
            ppu->t_coarse_x = data >> 3;
            ppu->ppuscroll = data;
            ppu->addr_latch = 1;
        }
        else {
            ppu->t_fine_y = data & 0x07;
            ppu->t_coarse_y = data >> 3;
            ppu->ppuscroll = data;
            ppu->addr_latch = 0;  
        }
        break;
    case 0x2006:
        if (ppu->addr_latch == 0) {
            ppu->addr_buffer = (unsigned short) data;
            ppu->addr_buffer <<= 8;
            ppu->addr_latch = 1;
        }
        else {
            ppu->addr_buffer |= (unsigned short) data;
            //what was stored in the bufffer gets stored in vram
            ppu->ppuaddr = ppu->addr_buffer;
            //tram is stored in vram
            ppu->coarse_x = ppu->t_coarse_x;
            ppu->coarse_y = ppu->t_coarse_y;
            ppu->nametable_x = ppu->t_nametable_x;
            ppu->nametable_y = ppu->t_nametable_y;
            ppu->fine_y = ppu->t_fine_y;
            ppu->addr_latch = 0;
        }
        break;
    case 0x2007:
        ppu->ppudata = data;
        ppu->mem[ppu->ppuaddr] = ppu->ppudata;
        ppu->ppuaddr += Ppu_getIncrement(ppu);
        break;
    default: //other registers aren't writeable
        break;
    }
}

unsigned int Ppu_getPixelVal(Ppu *ppu, unsigned char pixel, unsigned char pal) {
    unsigned int colored_pixel;
    colored_pixel = ppu->graphics.palColor[ppu->mem[(PALETTE_MEM_START + (pal << 2) + pixel)] & 0x3F];
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
    ppu->ppuctrl = 0;
    ppu->ppumask = 0;
    ppu->ppustatus = 0;
    ppu->oamaddr = 0;
    ppu->ppuscroll = 0;
    ppu->ppudata = 0;
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
    ppu->spriteZeroHitPossible = 0;
    ppu->spriteZeroBeingRendered = 0;
    ppu->pOam = (unsigned char*) ppu->oam;

    Graphics_init(&(ppu->graphics));
}

unsigned char flipbyte (unsigned char b) {
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    return b;
}

 void Ppu_tick(Ppu *ppu) {
    //on a visible scanline (or pre-render)
    if ((ppu->scanline >= -1) && (ppu->scanline < 240)) {
        //background rendering
        if ((ppu->scanline == 0) && (ppu->cycle == 0)) {
            //cycle 0 is skipped on scanline 0
            ppu->cycle = 1;
        }
        //on the pre-render scanline
        if ((ppu->scanline == -1) && (ppu->cycle == 1)) {
            //entering a new frame, clear:
            //VBlank, Sprite 0, Overflow, sprite shifters
            ppu->ppustatus &= 0x7F;
            int i;
            for (i = 0; i < 8; i++) {
                ppu->oam_pattern_shift_lo[i] = 0;
                ppu->oam_pattern_shift_hi[i] = 0;
            }
        }
        if (((ppu->cycle >= 1) && (ppu->cycle < 258)) || ((ppu->cycle >= 321) && (ppu->cycle < 338))) {
            //if BG is enabled in ppumask
            if ((ppu->ppumask & 0x08) == 0x08) {
                //shift BG shifters
                ppu->bg_shifter_pattern[0] <<= 1;
                ppu->bg_shifter_pattern[1] <<= 1;
                ppu->bg_shifter_palette[0] <<= 1;
                ppu->bg_shifter_palette[1] <<= 1;
            }
            // if FG is enabled in ppumask
            if (((ppu->ppumask & 0x10) == 0x10) && (ppu->cycle >= 1) && (ppu->cycle < 258)) {
                //shift FG shifters
                unsigned char i;
                for (i = 0; i < ppu->sprite_count; i++) {
                    if (ppu->scanline_sprites[i].x > 0)
                        ppu->scanline_sprites[i].x -= 1;
                    else {
                        ppu->oam_pattern_shift_lo[i] <<= 1;
                        ppu->oam_pattern_shift_hi[i] <<= 1;
                    }
                }
            }
            switch (ppu->cycle % 8) {
                case 1: //
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
                    break;
                case 2: //NT byte (cycle = 2)
                    //fetch next tile pattern
                    ppu->bg_next_tile_id = ppu->mem[0x2000 | (Ppu_getVram(ppu) & 0x0FFF)];
                    break;
                case 4: //AT byte (cycle = 4)
                    ppu->bg_next_tile_palette = ppu->mem[0x23C0 | ((unsigned short) ppu->nametable_y << 11) | ((unsigned short) ppu->nametable_x << 10) | ((ppu->coarse_y >> 2) << 3) | (ppu->coarse_x >> 2)];
                    //choose which one of the 4 2x2 tile sets
                    if (ppu->coarse_y & 0x02)
                        ppu->bg_next_tile_palette >>= 4;
                    if (ppu->coarse_x & 0x02)
                        ppu->bg_next_tile_palette >>= 2;
                    ppu->bg_next_tile_palette &= 0x03;
                    break;
                case 6: //Low BG tile byte (cycle = 6)
                    ppu->bg_next_tile_lsb = ppu->mem[(((unsigned short) ppu->ppuctrl & 0x0010) << 8) + ((unsigned short) ppu->bg_next_tile_id << 4) + (ppu->fine_y)];
                    break;
                case 0: //High BG tile byte (cycle = 8)
                    ppu->bg_next_tile_msb = ppu->mem[(((unsigned short) ppu->ppuctrl & 0x0010) << 8) + ((unsigned short) ppu->bg_next_tile_id << 4) + (ppu->fine_y) + 8];
                    //inc x
                    if ((ppu->ppumask & 0x08) | (ppu->ppumask & 0x10)) {
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
        }
        if (ppu->cycle == 256) {
            //if rendering is on
            if ((ppu->ppumask & 0x08) | (ppu->ppumask & 0x10)) {
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
            if ((ppu->ppumask & 0x08) | (ppu->ppumask & 0x10)) {
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
            if ((ppu->ppumask & 0x08) | (ppu->ppumask & 0x10)) {
                //finished frame, reset y coordinate each tick
                ppu->fine_y = ppu->t_fine_y;
                ppu->nametable_y = ppu->t_nametable_y;
                ppu->coarse_y = ppu->t_coarse_y;
            }
        }
        //foreground rendering
        if ((ppu->cycle == 257) && (ppu->scanline >= 0)) {
            //reached end of visible scanline
            //reset sprite count for next line
            ppu->sprite_count = 0;
            //clear sprites and shifters for next line
            unsigned char i;
            for (i = 0; i < 8; i++) {
                ppu->scanline_sprites[i].y = 0;
                ppu->scanline_sprites[i].attribute = 0;
                ppu->scanline_sprites[i].id = 0;
                ppu->scanline_sprites[i].x = 0;
                ppu->oam_pattern_shift_lo[i] = 0;
                ppu->oam_pattern_shift_hi[i] = 0;
            }

            //going to reavaluate sprite 0 possibility
            ppu->spriteZeroHitPossible = 0x00;

            //iterate to find first 8 sprites in next scanline
            unsigned char nOamEntry = 0;
            while ((nOamEntry < 64) && (ppu->sprite_count < 9)) {
                //calculate if sprite is visible
                short diff = ((unsigned short) ppu->scanline - (unsigned short)ppu->oam[nOamEntry].y);
                if ((diff >= 0) && (diff < ((ppu->ppuctrl & 0x20) ? 16 : 8))) {
                    //sprite is visible
                    if (ppu->sprite_count < 8) {
                        //check if there might be a sprite 0 hit
                        if (nOamEntry == 0) {  
                            ppu->spriteZeroHitPossible = 0x01;
                        }
                        //copy sprite from oam to next line sprites
                        ppu->scanline_sprites[ppu->sprite_count].y = ppu->oam[nOamEntry].y;
                        ppu->scanline_sprites[ppu->sprite_count].attribute = ppu->oam[nOamEntry].attribute;
                        ppu->scanline_sprites[ppu->sprite_count].id = ppu->oam[nOamEntry].id;
                        ppu->scanline_sprites[ppu->sprite_count].x = ppu->oam[nOamEntry].x;
                        ppu->sprite_count += 1;
                    }
                }
                nOamEntry += 1;
            }
            //finished evaluating next scanline sprites
            if (ppu->sprite_count > 8)
                ppu->ppustatus |= 0x20;
        }
        
        if (ppu->cycle == 340) {
            //end of scanline, preparing shifters with the next line sprites
            unsigned char i;
            for (i = 0; i < ppu->sprite_count; i++) {
                //find patterns for each sprite
                unsigned char sprite_pattern_bits_lo = 0;
                unsigned char sprite_pattern_bits_hi = 0;
                unsigned short sprite_pattern_addr_lo = 0;
                unsigned short sprite_pattern_addr_hi = 0;
                if ((ppu->ppuctrl & 0x20) == 0) {
                    //8x8 sprite mode
                    if ((ppu->scanline_sprites[i].attribute & 0x80) == 0) {
                        //sprite is not flipped vertically
                        sprite_pattern_addr_lo = (((unsigned short) ppu->ppuctrl & 0x0008) << 9) | (ppu->scanline_sprites[i].id << 4) | (ppu->scanline - ppu->scanline_sprites[i].y); 
                    }
                    else {
                        //sprite is flipped vertically
                        sprite_pattern_addr_lo = (((unsigned short) ppu->ppuctrl & 0x0008) << 9) | (ppu->scanline_sprites[i].id << 4) | (7 - ppu->scanline - ppu->scanline_sprites[i].y); 
                    }
                }
                else {
                    //8x16 sprite mode
                    if ((ppu->scanline_sprites[i].attribute & 0x80) == 0) {
                        //not flipped vertically
                        if (ppu->scanline - ppu->scanline_sprites[i].y < 8) {
                            //reading top half tile
                            sprite_pattern_addr_lo = (((ppu->scanline_sprites[i].id & 0x01) << 12) | ((ppu->scanline_sprites[i].id & 0xFE) << 4) | ((ppu->scanline - ppu->scanline_sprites[i].y) & 0x07));
                        }
                        else {
                            //reading bottom half tile
                            sprite_pattern_addr_lo = (((ppu->scanline_sprites[i].id & 0x01) << 12) | (((ppu->scanline_sprites[i].id & 0xFE) + 1) << 4) | ((ppu->scanline - ppu->scanline_sprites[i].y) & 0x07));
                        }
                    }
                    else {
                        //flipped vertically
                        if (ppu->scanline - ppu->scanline_sprites[i].y < 8) {
                            //reading top half tile
                            sprite_pattern_addr_lo = (((ppu->scanline_sprites[i].id & 0x01) << 12) | (((ppu->scanline_sprites[i].id & 0xFE) + 1) << 4) | ((7 - ppu->scanline - ppu->scanline_sprites[i].y) & 0x07));
                        }
                        else {
                            //reading bottom half tile
                            sprite_pattern_addr_lo = (((ppu->scanline_sprites[i].id & 0x01) << 12) | ((ppu->scanline_sprites[i].id & 0xFE) << 4) | ((7 - ppu->scanline - ppu->scanline_sprites[i].y) & 0x07));
                        }
                    }
                }
                sprite_pattern_addr_hi = sprite_pattern_addr_lo + 8;

                //read address to find the tiles
                sprite_pattern_bits_lo = ppu->mem[sprite_pattern_addr_lo];
                sprite_pattern_bits_hi = ppu->mem[sprite_pattern_addr_hi];
                
                //if the sprite is flipped, flip the pattern bytes
                if (ppu->scanline_sprites[i].attribute & 0x40) {
                    sprite_pattern_bits_lo = flipbyte(sprite_pattern_bits_lo);
                    sprite_pattern_bits_hi = flipbyte(sprite_pattern_bits_hi);
                }
                //load patterns into shift registers
                ppu->oam_pattern_shift_lo[i] = sprite_pattern_bits_lo;
                ppu->oam_pattern_shift_hi[i] = sprite_pattern_bits_hi;
            }
        }
    }

    //just exited visible scanline
    if ((ppu->scanline == 241) && (ppu->cycle == 1)) {
        //setting VBlank
        ppu->ppustatus |= 0x80;
        Ppu_drawPatterns(ppu);
        Ppu_draw(ppu);
        //emit nmi if ppucontrol nmi is set
        if ((ppu->ppuctrl & 0x80) == (0x80))
            ppu->nmi = 0x01;
    }

    //composing screen

    //background
    unsigned char bg_pixel = 0x00;
    unsigned char bg_palette = 0x00;
    //if rendering is on
    if ((ppu->ppumask & 0x08) == 0x08) {
        unsigned short bit_mux = 0x8000 >> ppu->fine_x;
        unsigned char p0_pixel = (ppu->bg_shifter_pattern[0] & bit_mux) > 0;
        unsigned char p1_pixel = (ppu->bg_shifter_pattern[1] & bit_mux) > 0;
        bg_pixel = (p1_pixel << 1) | p0_pixel;

        unsigned char bg_pal0 = (ppu->bg_shifter_palette[0] & bit_mux) > 0;
        unsigned char bg_pal1 = (ppu->bg_shifter_palette[1] & bit_mux) > 0;
        bg_palette = (bg_pal1 << 1) | bg_pal0;
    }

    //foreground
    unsigned char fg_pixel = 0;
    unsigned char fg_palette = 0;
    unsigned char fg_priority = 0;
    //if rendering is on
    if ((ppu->ppumask & 0x10) == 0x10) {
        ppu->spriteZeroBeingRendered = 0;
        unsigned char i;
        for (i = 0; i < ppu->sprite_count; i++) {
            if (ppu->scanline_sprites[i].x == 0) {
                unsigned char fg_pixel_lo = (ppu->oam_pattern_shift_lo[i] & 0x80) > 0;
                unsigned char fg_pixel_hi = (ppu->oam_pattern_shift_hi[i] & 0x80) > 0;
                fg_pixel = (fg_pixel_hi << 1) | (fg_pixel_lo);
                fg_palette = (ppu->scanline_sprites[i].attribute & 0x03) + 0x04;
                fg_priority = (ppu->scanline_sprites[i].attribute & 0x20) == 0;

                if (fg_pixel != 0) {
                    if (i == 0)
                        ppu->spriteZeroBeingRendered = 0x01;
                    break;
                }
            }
        }
    }

    //deciding the final pixel from bg and fg pixels
    unsigned char pixel = 0x00;
    unsigned char palette = 0x00;

    if ((bg_pixel == 0) && (fg_pixel == 0)) {
        pixel = 0x00;
        palette = 0x00;
    }
    if ((bg_pixel == 0) && (fg_pixel > 0)) {
        pixel = fg_pixel;
        palette = fg_palette;
    }
    if ((bg_pixel > 0) && (fg_pixel == 0)) {
        pixel = bg_pixel;
        palette = bg_palette;
    }
    if ((bg_pixel > 0) && (fg_pixel > 0)) {
        if (fg_priority) {
            //foreground has priority
            pixel = fg_pixel;
            palette = fg_palette;
        }
        else {
            pixel = bg_pixel;
            palette = bg_palette;
        }
        //sprite 0 hit detection
        if ((ppu->spriteZeroHitPossible & ppu->spriteZeroBeingRendered) == 0x01) {
            if (((ppu->ppumask & 0x08) == 0x08) && ((ppu->ppumask & 0x10) == 0x10)) { //check if both bg and fg are on
                if (~((ppu->ppumask & 0x02) | (ppu->ppumask & 0x04))) { 
                    if ((ppu->cycle >= 9) && (ppu->cycle < 258))
                        ppu->ppustatus |= 0x40;
                }
                else {
                    if ((ppu->cycle >= 1) && (ppu->cycle < 258))
                        ppu->ppustatus |= 0x40;
                }
            }
        }
    }

    if ((ppu->scanline >= 0) && (ppu->scanline <= 239) && (ppu->cycle >= 1) && (ppu->cycle <= 256)) {
        ppu->graphics.game_viewport[(ppu->cycle - 1) + (SCREEN_WIDTH * ppu->scanline)] = Ppu_getPixelVal(ppu, pixel, palette);
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