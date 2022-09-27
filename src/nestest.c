#include "nes.h"

int main(int argc, char *argv[]) {
  Nes nes;

  Nes_init(&nes);
  Cpu_mainLoop(nes->cpu);
  return 0;
}