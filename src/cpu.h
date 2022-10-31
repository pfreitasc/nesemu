#ifndef CPU_H
#define CPU_H


#include <unistd.h>
#include <stdio.h>
#include "ppu.h"
#include "controller.h"

//cpu constant definitions

#define CLOCK_FREQ 1790000
#define IRQ_START 0xFFFE
#define STACK_START 0x0100
#define DEBUG 1

enum addressMode {
    acc,
    absol,
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
typedef struct cpu {
    unsigned char a; //accumulator
    unsigned char x, y; //indexes
    unsigned short pc; //progmem counter
    unsigned char s; //stack pointer
    unsigned char p; //status register
    unsigned char mem[0xFFFF]; //cpu memory
    unsigned char cycleCounter; //number of cycles needed to execute current instruction
    unsigned char cycleDec; //gets decremented each cycle and updated on new instruction
    unsigned int totalCycles; //number of cycles elapsed since beginnig of emulation
    //dma regs and flags
    unsigned char dma_page;
    unsigned char dma_addr;
    unsigned char dma_data;
    unsigned char dma_dummy;
    unsigned char dma_transfer;
    //components
    Controller controller;
    Ppu ppu;
} Cpu;

//cpu functions definitions
void Cpu_powerUp(Cpu *cpu);
void Cpu_reset(Cpu *cpu);
void Cpu_loadRom(Cpu *cpu, unsigned char *prg_data);
void Cpu_setFlag(Cpu *cpu, int flag);
void Cpu_clearFlag(Cpu *cpu, int flag);
void Cpu_nmi(Cpu *cpu);
unsigned char Cpu_popStack(Cpu *cpu);
void Cpu_pushStack(Cpu *cpu, unsigned char data);
//unsigned char Cpu_time(Cpu *cpu);
void Cpu_write(Cpu *cpu, unsigned short addr, unsigned char data);
unsigned char Cpu_read(Cpu *cpu, unsigned short addr);
void Cpu_tick(Cpu *cpu);

#endif