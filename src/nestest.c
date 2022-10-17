#include "nes.h"

int main(int argc, char *argv[]) {
  Nes nes;

  Nes_init(&nes);
  Nes_mainLoop(&nes);
  return 0;
}