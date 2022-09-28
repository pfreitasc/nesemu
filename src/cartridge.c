#include "cartridge.h"
#include <stdio.h>

void Cartridge_getData(Cartridge *cartridge, char *filename) {
    FILE *fgame;
    fgame = fopen(filename, "rb");

    //checking if file opened
    if (fgame == NULL) {
        printf("Couldn't open the file: %s\n", filename);
        return;
    }

    fread(cartridge->header, 1, 16, fgame);

    fread(cartridge->prg_data, 1, cartridge->header[4] * 16384, fgame);
    //MMAP0: mirror 16 KiB if its a 16 KiB rom
    if (cartridge->header[4] == 1) {
        int i;
        for (i = 0; i < 16384; i++) {
            cartridge->prg_data[16384 + i] = cartridge->prg_data[i];
        }
    }

    fread(cartridge->chr_data, 1, cartridge->header[5] * 8192, fgame);
    fclose(fgame);
}