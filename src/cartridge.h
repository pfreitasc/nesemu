#ifndef CARTRIDGE_H
#define CARTRIDGE_H

typedef struct {
    unsigned char header[16];
    unsigned char prg_data[16384 * 15];
    unsigned char chr_data[8192 * 15];
} Cartridge;

//function prototypes
void Cartridge_getData(Cartridge *cartridge, char *filename);

#endif