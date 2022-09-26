#ifndef CPU_H
#define CPU_H


#include <unistd.h>
#include <stdio.h>

//cpu constant definitions

#define CLOCK_FREQ 1790000
#define IRQ_START 0xFFFE
#define STACK_START 0x0100

enum addressMode {
    acc,
    abs,
    absx,
    absy,
    imm,
    impl,
    ind,
    indx,
    indy,
    rel,
    zpg,
    zpgx,
    zpgy
};

enum flag {
    N, //negative
    V, //oveflow
    Ignored,
    B, //break
    D, //decimal (use BCD for arithmetics)
    I, //interrupt (IRQ disable)
    Z, //zero
    C //carry
};


//cpu struct definition
typedef struct {
    unsigned char a; //accumulator
    unsigned char x, y; //indexes
    unsigned short pc; //program counter
    unsigned short s; //stack pointer
    unsigned char p; //status register
    unsigned short ram[0xFFFF]; //cpu memory
    unsigned int cycleCounter; //number of cycles needed to execute current instruction
} Cpu;

//cpu functions definitions
void Cpu_powerUp(Cpu *cpu);
void Cpu_reset(Cpu *cpu);
void Cpu_loadRam(Cpu *cpu, char *filename);
void Cpu_setFlag(Cpu *cpu, int flag);
void Cpu_clearFlag(Cpu *cpu, int flag);
void Cpu_decode(Cpu *cpu);
void Cpu_tick(Cpu *cpu);
void Cpu_mainLoop(Cpu *cpu);

#endif