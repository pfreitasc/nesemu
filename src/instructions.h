#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "cpu.h"

//auxiliary functions
unsigned char fetchOperand(Cpu *cpu, int addr_mode);
unsigned short fetchAddr(Cpu *cpu, int addr_mode);
void pushStack(Cpu *cpu, unsigned char val);
unsigned char popStack(Cpu *cpu);

//address mode fuctions
unsigned char mode_acc(Cpu *cpu);
unsigned short mode_abs(Cpu *cpu);
unsigned short mode_absx(Cpu *cpu);
unsigned short mode_absy(Cpu *cpu);
unsigned char mode_imm(Cpu *cpu);
unsigned char mode_impl(Cpu *cpu);
unsigned short mode_ind(Cpu *cpu);
unsigned short mode_indx(Cpu *cpu);
unsigned short mode_indy(Cpu *cpu);
unsigned short mode_rel(Cpu *cpu);
unsigned short mode_zpg(Cpu *cpu);
unsigned short mode_zpgx(Cpu *cpu);
unsigned short mode_zpgy(Cpu *cpu);

//instructions
//descriptions https://www.masswerk.at/6502/6502_instruction_set.html
void ADC(Cpu *cpu, int addr_mode);
void AND(Cpu *cpu, int addr_mode);
void ASL(Cpu *cpu, int addr_mode);
void BCC(Cpu *cpu, int addr_mode);
void BCS(Cpu *cpu, int addr_mode);
void BEQ(Cpu *cpu, int addr_mode);
void BIT(Cpu *cpu, int addr_mode);
void BMI(Cpu *cpu, int addr_mode);
void BNE(Cpu *cpu, int addr_mode);
void BPL(Cpu *cpu, int addr_mode);
void BRK(Cpu *cpu, int addr_mode);
void BVC(Cpu *cpu, int addr_mode);
void BVS(Cpu *cpu, int addr_mode);
void CLC(Cpu *cpu, int addr_mode);
void CLD(Cpu *cpu, int addr_mode);
void CLI(Cpu *cpu, int addr_mode);
void CLV(Cpu *cpu, int addr_mode);
void CMP(Cpu *cpu, int addr_mode);
void CPX(Cpu *cpu, int addr_mode);
void CPY(Cpu *cpu, int addr_mode);
void DEC(Cpu *cpu, int addr_mode);
void DEX(Cpu *cpu, int addr_mode);
void DEY(Cpu *cpu, int addr_mode);
void EOR(Cpu *cpu, int addr_mode);
void INC(Cpu *cpu, int addr_mode);
void INX(Cpu *cpu, int addr_mode);
void INY(Cpu *cpu, int addr_mode);
void JMP(Cpu *cpu, int addr_mode);
void JSR(Cpu *cpu, int addr_mode);
void LDA(Cpu *cpu, int addr_mode);
void LDX(Cpu *cpu, int addr_mode);
void LDY(Cpu *cpu, int addr_mode);
void LSR(Cpu *cpu, int addr_mode);
void NOP(Cpu *cpu, int addr_mode);
void ORA(Cpu *cpu, int addr_mode);
void PHA(Cpu *cpu, int addr_mode);
void PHP(Cpu *cpu, int addr_mode);
void PLA(Cpu *cpu, int addr_mode);
void PLP(Cpu *cpu, int addr_mode);
void PLP(Cpu *cpu, int addr_mode);
void ROL(Cpu *cpu, int addr_mode);
void ROR(Cpu *cpu, int addr_mode);
void RTI(Cpu *cpu, int addr_mode);
void RTS(Cpu *cpu, int addr_mode);
void SBC(Cpu *cpu, int addr_mode);
void SEC(Cpu *cpu, int addr_mode);
void SED(Cpu *cpu, int addr_mode);
void SEI(Cpu *cpu, int addr_mode);
void STA(Cpu *cpu, int addr_mode);
void STX(Cpu *cpu, int addr_mode);
void STY(Cpu *cpu, int addr_mode);
void TAX(Cpu *cpu, int addr_mode);
void TAY(Cpu *cpu, int addr_mode);
void TSX(Cpu *cpu, int addr_mode);
void TXA(Cpu *cpu, int addr_mode);
void TXS(Cpu *cpu, int addr_mode);
void TYA(Cpu *cpu, int addr_mode);
void XXX();

#endif