#include "cpu.h"

int main(int argc, char *argv[]) {
  Cpu cpu;

  Cpu_powerUp(&cpu);
  Cpu_loadRam(&cpu, "../roms/6502_functional_test.bin");
  Cpu_mainLoop(&cpu);
  return 0;
}