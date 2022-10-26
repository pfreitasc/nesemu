#include "cpu.h"
#include "ppu.h"
#include "instructions.h"

void Cpu_powerUp(Cpu *cpu) {
    cpu->a = 0;
    cpu->x = 0;
    cpu->y = 0;
    cpu->s = 0xFD;
    cpu->pc = 0;
    cpu->p = 0x24;
    cpu->totalCycles = 7;

    int i;
    for (i = 0; i < 0xFFFF; i++)
        cpu->mem[i] = 0;

    //initializing ppu
    Ppu_init(&(cpu->ppu));
}

void Cpu_reset(Cpu *cpu) {
    //a, x, y are not affected
    //s is decremented by 3
    cpu->s -= 3;
    //enable interrupt
    cpu->p |= 0x04;
}

unsigned char Cpu_read(Cpu *cpu, unsigned short addr) {
    unsigned char data = cpu->mem[addr];
    //printf("%02X ", data);
    //if reading from ppu registers
    if (((addr >= 0x2000) && (addr <= 0x2007)) || (addr == 4014)) {
        data = Ppu_read(&(cpu->ppu), addr);
    }
    return data;
}

void Cpu_write(Cpu *cpu, unsigned short addr, unsigned char data) {
    cpu->mem[addr] = data;
    //printf("%02X ", data);
    //if writing to ppu registers
    if (((addr >= 0x2000) && (addr <= 0x2007)) | (addr == 4014)) {
        //send data do ppu
        Ppu_write(&(cpu->ppu), data, addr);
    }
}

void Cpu_loadRom(Cpu *cpu, unsigned char *prg_data) {
    //mapper 0
    int i;
    for (i = 0; i < 16384 * 2; i++) {
        cpu->mem[0x8000 + i] = prg_data[i];
    }

    //pc points to address in reset vector FFFC-FFFD
    unsigned short addr_l = cpu->mem[0xFFFC];
    unsigned short addr_h = cpu->mem[0xFFFD];
    addr_h <<= 8;
    unsigned short addr = addr_l | addr_h;
    cpu->pc = addr;

}

void Cpu_setFlag(Cpu *cpu, int flag) {
    unsigned char mask = 0x80;
    mask = mask >> flag;
    cpu->p |= mask;
}

void Cpu_clearFlag(Cpu *cpu, int flag) {
    unsigned char mask = 0x80;
    mask = mask >> flag;
    mask = ~mask;
    cpu->p &= mask;
}

void Cpu_pushStack(Cpu *cpu, unsigned char val){
    cpu->mem[STACK_START + cpu->s] = val;
    cpu->s -= 1;
}

unsigned char Cpu_popStack(Cpu *cpu){
    cpu->s += 1;
    return cpu->mem[STACK_START + cpu->s];
}

void Cpu_irq(Cpu *cpu) {
    if ((cpu->p & 0x04) == 0x04) {
        return;
    }

    //push pc to stack
    unsigned short curr_addr = cpu->pc;
    unsigned char curr_addr_l = curr_addr & 0x00FF;
    curr_addr = curr_addr >> 8;
    unsigned char curr_addr_h = curr_addr & 0x00FF;

    Cpu_pushStack(cpu, curr_addr_h);
    Cpu_pushStack(cpu, curr_addr_l);

    //push status to stack
    cpu->p &= (0 >> B);
    cpu->p |= (1 >> Ignored);
    cpu->p |= (1 >> I);
    Cpu_pushStack(cpu, cpu->p);

    unsigned short addr_h = cpu->mem[0xFFFF];
    addr_h <<= 8;
    unsigned short addr_l = cpu->mem[0xFFFE];
    unsigned short addr = addr_l | addr_h;
    cpu->pc = addr;

    //timing adjustment
    cpu->cycleCounter = 7;  
}

void Cpu_nmi(Cpu *cpu) {

    //push pc to stack
    unsigned short curr_addr = cpu->pc;
    unsigned char curr_addr_l = curr_addr & 0x00FF;
    curr_addr = curr_addr >> 8;
    unsigned char curr_addr_h = curr_addr & 0x00FF;

    Cpu_pushStack(cpu, curr_addr_h);
    Cpu_pushStack(cpu, curr_addr_l);

    //push status to stack
    cpu->p &= (0 >> B);
    cpu->p |= (1 >> Ignored);
    cpu->p |= (1 >> I);
    Cpu_pushStack(cpu, cpu->p);

    unsigned short addr_l = cpu->mem[0xFFFA];
    unsigned short addr_h = cpu->mem[0xFFFB];
    addr_h <<= 8;
    unsigned short addr = addr_l | addr_h;
    cpu->pc = addr;

    //timing adjustment
    cpu->cycleCounter = 8;
}

void Cpu_decode(Cpu *cpu) {
    #ifdef DEBUG
    printf("%04X  ", cpu->pc);
    #endif
    //fetch opcode
    unsigned char opcode = Cpu_read(cpu, cpu->pc);
    cpu->pc += 1;

    //reset number of cycles counter
    cpu->cycleCounter = 0;

    //decode table from https://www.masswerk.at/6502/6502_instruction_set.html
    switch (opcode) {
        //row 0
        case 0x00:
            BRK(cpu, impl);
            break;
        case 0x01:
            ORA(cpu, indx);
            break;
        case 0x05:
            ORA(cpu, zpg);
            break;
        case 0x06:
            ASL(cpu, zpg);
            break;
        case 0x08:
            PHP(cpu, impl);
            break;
        case 0x09:
            ORA(cpu, imm);
            break;
        case 0x0a:
            ASL(cpu, acc);
            break;
        case 0x0d:
            ORA(cpu, absol);
            break;
        case 0x0e:
            ASL(cpu, absol);
            break;
        //row 1
        case 0x10:
            BPL(cpu, rel);
            break;
        case 0x11:
            ORA(cpu, indy);
            break;
        case 0x15:
            ORA(cpu, zpgx);
            break;
        case 0x16:
            ASL(cpu, zpgx);
            break;
        case 0x18:
            CLC(cpu, impl);
            break;
        case 0x19:
            ORA(cpu, absy);
            break;
        case 0x1d:
            ORA(cpu, absx);
            break;
        case 0x1e:
            ASL(cpu, absx);
            break;
        //row 2
        case 0x20:
            JSR(cpu, absol);
            break;
        case 0x21:
            AND(cpu, indx);
            break;
        case 0x24:
            BIT(cpu, zpg);
            break;
        case 0x25:
            AND(cpu, zpg);
            break;
        case 0x26:
            ROL(cpu, zpg);
            break;
        case 0x28:
            PLP(cpu, impl);
            break;
        case 0x29:
            AND(cpu, imm);
            break;
        case 0x2a:
            ROL(cpu, acc);
            break;
        case 0x2c:
            BIT(cpu, absol);
            break;
        case 0x2d:
            AND(cpu, absol);
            break;
        case 0x2e:
            ROL(cpu, absol);
            break;
        //row 3
        case 0x30:
            BMI(cpu, rel);
            break;
        case 0x31:
            AND(cpu, indy);
            break;
        case 0x35:
            AND(cpu, zpgx);
            break;
        case 0x36:
            ROL(cpu, zpgx);
            break;
        case 0x38:
            SEC(cpu, impl);
            break;
        case 0x39:
            AND(cpu, absy);
            break;
        case 0x3d:
            AND(cpu, absx);
            break;
        case 0x3e:
            ROL(cpu, absx);
            break;
        //row 4
        case 0x40:
            RTI(cpu, impl);
            break;
        case 0x41:
            EOR(cpu, indx);
            break;
        case 0x45:
            EOR(cpu, zpg);
            break;
        case 0x46:
            LSR(cpu, zpg);
            break;
        case 0x48:
            PHA(cpu, impl);
            break;
        case 0x49:
            EOR(cpu, imm);
            break;
        case 0x4a:
            LSR(cpu, acc);
            break;
        case 0x4c:
            JMP(cpu, absol);
            break;
        case 0x4d:
            EOR(cpu, absol);
            break;
        case 0x4e:
            LSR(cpu, absol);
            break;
        //row 5
        case 0x50:
            BVC(cpu, rel);
            break;
        case 0x51:
            EOR(cpu, indy);
            break;
        case 0x55:
            EOR(cpu, zpgx);
            break;
        case 0x56:
            LSR(cpu, zpgx);
            break;
        case 0x58:
            CLI(cpu, impl);
            break;
        case 0x59:
            EOR(cpu, absy);
            break;
        case 0x5d:
            EOR(cpu, absx);
            break;
        case 0x5e:
            LSR(cpu, absx);
            break;
        //row 6
        case 0x60:
            RTS(cpu, impl);
            break;
        case 0x61:
            ADC(cpu, indx);
            break;
        case 0x65:
            ADC(cpu, zpg);
            break;
        case 0x66:
            ROR(cpu, zpg);
            break;
        case 0x68:
            PLA(cpu, impl);
            break;
        case 0x69:
            ADC(cpu, imm);
            break;
        case 0x6a:
            ROR(cpu, acc);
            break;
        case 0x6c:
            JMP(cpu, ind);
            break;
        case 0x6d:
            ADC(cpu, absol);
            break;
        case 0x6e:
            ROR(cpu, absol);
            break;
        //row 7
        case 0x70:
            BVS(cpu, rel);
            break;
        case 0x71:
            ADC(cpu, indy);
            break;
        case 0x75:
            ADC(cpu, zpgx);
            break;
        case 0x76:
            ROR(cpu, zpgx);
            break;
        case 0x78:
            SEI(cpu, impl);
            break;
        case 0x79:
            ADC(cpu, absy);
            break;
        case 0x7d:
            ADC(cpu, absx);
            break;
        case 0x7e:
            ROR(cpu, absx);
            break;
        //row 8
        case 0x81:
            STA(cpu, indx);
            break;
        case 0x84:
            STY(cpu, zpg);
            break;
        case 0x85:
            STA(cpu, zpg);
            break;
        case 0x86:
            STX(cpu, zpg);
            break;
        case 0x88:
            DEY(cpu, impl);
            break;
        case 0x8a:
            TXA(cpu, impl);
            break;
        case 0x8c:
            STY(cpu, absol);
            break;
        case 0x8d:
            STA(cpu, absol);
            break;
        case 0x8e:
            STX(cpu, absol);
            break;
        //row 9
        case 0x90:
            BCC(cpu, rel);
            break;
        case 0x91:
            STA(cpu, indy);
            break;
        case 0x94:
            STY(cpu, zpgx);
            break;
        case 0x95:
            STA(cpu, zpgx);
            break;
        case 0x96:
            STX(cpu, zpgy);
            break;
        case 0x98:
            TYA(cpu, impl);
            break;
        case 0x99:
            STA(cpu, absy);
            break;
        case 0x9a:
            TXS(cpu, impl);
            break;
        case 0x9d:
            STA(cpu, absx);
            break;
        //row a
        case 0xa0:
            LDY(cpu, imm);
            break;
        case 0xa1:
            LDA(cpu, indx);
            break;
        case 0xa2:
            LDX(cpu, imm);
            break;
        case 0xa4:
            LDY(cpu, zpg);
            break;
        case 0xa5:
            LDA(cpu, zpg);
            break;
        case 0xa6:
            LDX(cpu, zpg);
            break;
        case 0xa8:
            TAY(cpu, impl);
            break;
        case 0xa9:
            LDA(cpu, imm);
            break;
        case 0xaa:
            TAX(cpu, impl);
            break;
        case 0xac:
            LDY(cpu, absol);
            break;
        case 0xad:
            LDA(cpu, absol);
            break;
        case 0xae:
            LDX(cpu, absol);
            break;
        //row b
        case 0xb0:
            BCS(cpu, rel);
            break;
        case 0xb1:
            LDA(cpu, indy);
            break;
        case 0xb4:
            LDY(cpu, zpgx);
            break;
        case 0xb5:
            LDA(cpu, zpgx);
            break;
        case 0xb6:
            LDX(cpu, zpgy);
            break;
        case 0xb8:
            CLV(cpu, impl);
            break;
        case 0xb9:
            LDA(cpu, absy);
            break;
        case 0xba:
            TSX(cpu, impl);
            break;
        case 0xbc:
            LDY(cpu, absx);
            break;
        case 0xbd:
            LDA(cpu, absx);
            break;
        case 0xbe:
            LDX(cpu, absy);
            break;
        //row c
        case 0xc0:
            CPY(cpu, imm);
            break;
        case 0xc1:
            CMP(cpu, indx);
            break;
        case 0xc4:
            CPY(cpu, zpg);
            break;
        case 0xc5:
            CMP(cpu, zpg);
            break;
        case 0xc6:
            DEC(cpu, zpg);
            break;
        case 0xc8:
            INY(cpu, impl);
            break;
        case 0xc9:
            CMP(cpu, imm);
            break;
        case 0xca:
            DEX(cpu, impl);
            break;
        case 0xcc:
            CPY(cpu, absol);
            break;
        case 0xcd:
            CMP(cpu, absol);
            break;
        case 0xce:
            DEC(cpu, absol);
            break;
        //row d
        case 0xd0:
            BNE(cpu, rel);
            break;
        case 0xd1:
            CMP(cpu, indy);
            break;
        case 0xd5:
            CMP(cpu, zpgx);
            break;
        case 0xd6:
            DEC(cpu, zpgx);
            break;
        case 0xd8:
            CLD(cpu, impl);
            break;
        case 0xd9:
            CMP(cpu, absy);
            break;
        case 0xdd:
            CMP(cpu, absx);
            break;
        case 0xde:
            DEC(cpu, absx);
            break;
        //row e
        case 0xe0:
            CPX(cpu, imm);
            break;
        case 0xe1:
            SBC(cpu, indx);
            break;
        case 0xe4:
            CPX(cpu, zpg);
            break;
        case 0xe5:
            SBC(cpu, zpg);
            break;
        case 0xe6:
            INC(cpu, zpg);
            break;
        case 0xe8:
            INX(cpu, impl);
            break;
        case 0xe9:
            SBC(cpu, imm);
            break;
        case 0xea:
            NOP(cpu, impl);
            break;
        case 0xec:
            CPX(cpu, absol);
            break;
        case 0xed:
            SBC(cpu, absol);
            break;
        case 0xee:
            INC(cpu, absol);
            break;
        //row f
        case 0xf0:
            BEQ(cpu, rel);
            break;
        case 0xf1:
            SBC(cpu, indy);
            break;
        case 0xf5:
            SBC(cpu, zpgx);
            break;
        case 0xf6:
            INC(cpu, zpgx);
            break;
        case 0xf8:
            SED(cpu, impl);
            break;
        case 0xf9:
            SBC(cpu, absy);
            break;
        case 0xfd:
            SBC(cpu, absx);
            break;
        case 0xfe:
            INC(cpu, absx);
            break;
        default:
            XXX();
            break;
    }
    if (cpu->ppu.nmi) {
        Cpu_nmi(cpu);
        cpu->ppu.nmi = 0;
    }
}

unsigned char Cpu_time(Cpu *cpu){
    float cycle_time = 1 / ((float) CLOCK_FREQ); //in seconds
    usleep(cpu->cycleCounter * cycle_time * 1000000);
    cpu->totalCycles += cpu->cycleCounter;
    return cpu->cycleCounter;
}