#include "instructions.h"

//more info on https://www.masswerk.at/6502/6502_instruction_set.html

//address mode fuctions
unsigned char mode_acc(Cpu *cpu) {
    cpu->cycleCounter += 2;
    return 0;
}

unsigned short mode_abs(Cpu *cpu) {
    unsigned char ll = cpu->ram[cpu->pc];
    cpu->pc += 1;
    unsigned char hh = cpu->ram[cpu->pc];
    cpu->pc += 1;

    unsigned short addr = 0 | hh;
    addr = addr << 8;
    addr |= ll;

    cpu->cycleCounter += 4;

    return addr;
}

unsigned short mode_absx(Cpu *cpu) {
    unsigned char ll = cpu->ram[(cpu->pc) + (cpu->x)];
    cpu->pc += 1;
    unsigned char hh = cpu->ram[(cpu->pc) + (cpu->x)];
    cpu->pc += 1;

    unsigned short addr = 0 | hh;
    addr = addr << 8;
    addr |= ll;

    cpu->cycleCounter += 4;

    if ((addr >> 8) != (cpu->pc))
        cpu->cycleCounter += 1;

    return addr;
}

unsigned short mode_absy(Cpu *cpu) {
    unsigned char ll = cpu->ram[(cpu->pc) + (cpu->y)];
    cpu->pc += 1;
    unsigned char hh = cpu->ram[(cpu->pc) + (cpu->y)];
    cpu->pc += 1;

    unsigned short addr = 0 | hh;
    addr = addr << 8;
    addr |= ll;

    cpu->cycleCounter += 4;


    if ((addr >> 8) != (cpu->pc))
        cpu->cycleCounter += 1;

    return addr;
}

unsigned char mode_imm(Cpu *cpu) {
    unsigned char operand = cpu->ram[cpu->pc];
    cpu->pc += 1;

    cpu->cycleCounter += 2;

    return operand;
}

unsigned char mode_impl(Cpu *cpu) {
    cpu->cycleCounter += 2;
    return 0;
}

unsigned short mode_ind(Cpu *cpu) {
    unsigned char pointer_ll = cpu->ram[cpu->pc];
    cpu->pc += 1;

    unsigned char pointer_hh = cpu->ram[cpu->pc];
    cpu->pc += 1;
    
    unsigned short pointer = 0 | pointer_hh;  
    pointer = pointer << 8;
    pointer |= pointer_ll;

    unsigned short addr = 0;
    //6502 bug instead of taking from last of page and first of next page
    //it takes from last of page and first of same page
    //ex: instead of reading from $C0FF/$C100 it reads from $C0FF/$C000
    if ((pointer & 0xFF) == 0xFF) {
        addr |= cpu->ram[pointer];
        addr = addr << 8;
        addr |= cpu->ram[(pointer & 0xFF00)];
    }
    else {
        addr |= cpu->ram[pointer];
        addr = addr << 8;
        addr |= cpu->ram[pointer + 1];
    }

    cpu->cycleCounter += 5;

    return addr;
}

unsigned short mode_indx(Cpu *cpu) {
    unsigned char zpg_pointer = cpu->ram[cpu->pc];
    cpu->pc +=1;

    unsigned char addr_ll = cpu->ram[zpg_pointer + cpu->x] & 0xFF;
    unsigned char addr_hh = cpu->ram[zpg_pointer + cpu->x + 1] & 0xFF;
    
    unsigned short addr = 0 | addr_hh;
    addr = addr << 8;
    addr |= addr_ll;

    cpu->cycleCounter += 6;

    return addr;
}

unsigned short mode_indy(Cpu *cpu) {
    unsigned char zpg_pointer = cpu->ram[cpu->pc];
    cpu->pc +=1;

    unsigned char addr_ll = cpu->ram[zpg_pointer] & 0xFF;
    unsigned char addr_hh = cpu->ram[zpg_pointer + 1] & 0xFF;
    
    unsigned short addr = 0 | addr_hh;
    addr = addr << 8;
    addr |= addr_ll;
    addr += cpu->y;

    cpu->cycleCounter += 5;

    if ((addr >> 8) != (cpu->pc))
        cpu->cycleCounter += 1;

    return addr;
}

unsigned short mode_rel(Cpu *cpu) {
    char offset = cpu->ram[cpu->pc];
    unsigned short addr;

    cpu->pc += 1;

    if ((offset & 0x80) == 0x80) {
        offset = ~offset + 1;
        addr = cpu->pc - offset;
    }
    else {
        addr = cpu->pc + offset;
    }

    cpu->cycleCounter += 2;

    if ((addr && 0xFF00) != (cpu->pc && 0xFF00))
        cpu->cycleCounter += 1;

    return addr;
}

unsigned short mode_zpg(Cpu *cpu) {
    unsigned char ll = cpu->ram[cpu->pc];
    cpu->pc += 1;

    unsigned short addr = 0 | ll;

    cpu->cycleCounter += 3;

    return addr;
}

unsigned short mode_zpgx(Cpu *cpu) {
    unsigned char ll = cpu->ram[(cpu->pc) + (cpu->x)];
    cpu->pc += 1;

    unsigned short addr = 0 | ll;

    cpu->cycleCounter += 4;

    return addr;
}

unsigned short mode_zpgy(Cpu *cpu) {
    unsigned char ll = cpu->ram[(cpu->pc) + (cpu->y)];
    cpu->pc += 1;

    unsigned short addr = 0 | ll;

    cpu->cycleCounter += 4;

    return addr;
}

//auxiliary functions
unsigned short fetchAddr(Cpu *cpu, int addr_mode) {
    unsigned short addr;
    switch (addr_mode) {
        case abs:
            addr = mode_abs(cpu);
            break;
        case absx:
            addr = mode_absx(cpu);
            break;
        case absy:
            addr = mode_absy(cpu);
            break;
        case ind:
            addr = mode_ind(cpu);
            break;
        case indx:
            addr = mode_indx(cpu);
            break;
        case indy:
            addr = mode_indy(cpu);
            break;
        case rel:
            addr = mode_rel(cpu);
            break;
        case zpg:
            addr = mode_zpg(cpu);
            break;
        case zpgx:
            addr = mode_zpgx(cpu);
            break;
        case zpgy:
            addr = mode_zpgy(cpu);
            break;
        default:
            break;
    }
    return addr;
}

unsigned char fetchOperand(Cpu *cpu, int addr_mode) {
    unsigned char operand;
    switch (addr_mode) {
        case acc:
            operand = mode_acc(cpu);
            break;
        case imm:
            operand = mode_imm(cpu);
            break;
        case impl:
            operand = mode_impl(cpu);
            break;
        default:
            operand = cpu->ram[fetchAddr(cpu, addr_mode)];
    }
    return operand;
}

void pushStack(Cpu *cpu, unsigned char val){
    cpu->ram[STACK_START + cpu->s] = val;
    cpu->s -= 1;
}

unsigned char popStack(Cpu *cpu){
    cpu->s += 1;
    return cpu->ram[STACK_START + cpu->s];
}

//instructions
//transfer instructions
void LDA(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("LDA ");
    #endif
    cpu->a = fetchOperand(cpu, addr_mode);
    
    //setting Z and N flags
    if (cpu->a < 0)
        Cpu_setFlag(cpu, N);
    else
        Cpu_clearFlag(cpu, N);
    if (cpu->a == 0)
        Cpu_setFlag(cpu, Z);
    else 
        Cpu_clearFlag(cpu, Z);
}

void LDX(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("LDX ");
    #endif
    cpu->x = fetchOperand(cpu, addr_mode);
    
    //setting Z and N flags
    if (cpu->x < 0)
        Cpu_setFlag(cpu, N);
    else
        Cpu_clearFlag(cpu, N);
    if (cpu->x == 0)
        Cpu_setFlag(cpu, Z);
    else
        Cpu_clearFlag(cpu, Z);
}

void LDY(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("LDY ");
    #endif
    cpu->y = fetchOperand(cpu, addr_mode);
    
    //setting Z and N flags
    if (cpu->y < 0)
        Cpu_setFlag(cpu, N);
    else
        Cpu_clearFlag(cpu, N);
    if (cpu->y == 0)
        Cpu_setFlag(cpu, Z);
    else 
        Cpu_clearFlag(cpu, Z);
}

void STA(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("STA ");
    #endif
    unsigned short addr = fetchAddr(cpu, addr_mode);
    cpu->ram[addr] = cpu->a;

    //timing adjustments
    if (addr_mode == absx)
        cpu->cycleCounter = 5;
    else if (addr_mode == absy)
        cpu->cycleCounter = 5;
    else if (addr_mode == indy)
        cpu->cycleCounter = 6;
}

void STX(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("STX ");
    #endif
    unsigned short addr = fetchAddr(cpu, addr_mode);
    cpu->ram[addr] = cpu->x;
}

void STY(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("STY ");
    #endif
    unsigned short addr = fetchAddr(cpu, addr_mode);
    cpu->ram[addr] = cpu->y;
}

void TAX(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("TAX ");
    #endif
    fetchOperand(cpu, addr_mode);
    cpu->x = cpu->a;

    //setting Z and N flags
    if (cpu->x < 0)
        Cpu_setFlag(cpu, N);
    else
        Cpu_clearFlag(cpu, N);
    
    if (cpu->x == 0)
        Cpu_setFlag(cpu, Z);
    else 
        Cpu_clearFlag(cpu, Z);
}

void TAY(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("TAY ");
    #endif
    fetchOperand(cpu, addr_mode);
    cpu->y = cpu->a;
    
    //setting Z and N flags
    if (cpu->y < 0)
        Cpu_setFlag(cpu, N);
    else
        Cpu_clearFlag(cpu, N);
    
    if (cpu->y == 0)
        Cpu_setFlag(cpu, Z);
    else 
        Cpu_clearFlag(cpu, Z);
}

void TSX(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("TSX ");
    #endif
    fetchOperand(cpu, addr_mode);
    cpu->x = cpu->s;

    //setting Z and N flags
    if (cpu->x < 0)
        Cpu_setFlag(cpu, N);
    else
        Cpu_clearFlag(cpu, N);
    
    if (cpu->x == 0)
        Cpu_setFlag(cpu, Z);
    else 
        Cpu_clearFlag(cpu, Z);
}

void TXA(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("TXA ");
    #endif
    fetchOperand(cpu, addr_mode);
    cpu->a = cpu->x;

    //setting Z and N flags
    if (cpu->a < 0)
        Cpu_setFlag(cpu, N);
    else
        Cpu_clearFlag(cpu, N);
    
    if (cpu->a == 0)
        Cpu_setFlag(cpu, Z);
    else 
        Cpu_clearFlag(cpu, Z);
}

void TXS(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("TXS ");
    #endif
    fetchOperand(cpu, addr_mode);
    cpu->s = cpu->x;
}

void TYA(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("TYA ");
    #endif
    fetchOperand(cpu, addr_mode);
    cpu->a = cpu->y;

    //setting Z and N flags
    if (cpu->a < 0)
        Cpu_setFlag(cpu, N);
    else
        Cpu_clearFlag(cpu, N);
    
    if (cpu->a == 0)
        Cpu_setFlag(cpu, Z);
    else 
        Cpu_clearFlag(cpu, Z);
}

//stack instructions
void PHA(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("PHA ");
    #endif
    fetchOperand(cpu, addr_mode);
    pushStack(cpu, cpu->a);
}

void PHP(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("PHP ");
    #endif
    fetchOperand(cpu, addr_mode);
    pushStack(cpu, cpu->p);
}

void PLA(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("PLA ");
    #endif
    fetchOperand(cpu, addr_mode);
    cpu->a = popStack(cpu);

    //setting Z and N flags
    if (cpu->a < 0)
        Cpu_setFlag(cpu, N);
    else
        Cpu_clearFlag(cpu, N);
    
    if (cpu->a == 0)
        Cpu_setFlag(cpu, Z);
    else 
        Cpu_clearFlag(cpu, Z);
}

void PLP(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("PLP ");
    #endif
    fetchOperand(cpu, addr_mode);
    cpu->p = popStack(cpu);
}

//decrements & increments
void DEC(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("DEC ");
    #endif
    unsigned short addr = fetchAddr(cpu, addr_mode);
    cpu->ram[addr] -= 1;
    
    //setting Z and N flags
    if (cpu->ram[addr] < 0)
        Cpu_setFlag(cpu, N);
    else
        Cpu_clearFlag(cpu, N);
    
    if (cpu->ram[addr] == 0)
        Cpu_setFlag(cpu, Z);
    else 
        Cpu_clearFlag(cpu, Z);

    //timing adjustments
    if (addr_mode == absx)
        cpu->cycleCounter = 7;
    else
        cpu->cycleCounter += 2;
}

void DEX(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("DEX ");
    #endif
    fetchOperand(cpu, addr_mode);
    cpu->x -= 1;
    
    //setting Z and N flags
    if (cpu->x < 0)
        Cpu_setFlag(cpu, N);
    else
        Cpu_clearFlag(cpu, N);
    
    if (cpu->x == 0)
        Cpu_setFlag(cpu, Z);
    else 
        Cpu_clearFlag(cpu, Z);
}

void DEY(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("DEY ");
    #endif
    fetchOperand(cpu, addr_mode);
    cpu->x -= 1;
    
    //setting Z and N flags
    if (cpu->y < 0)
        Cpu_setFlag(cpu, N);
    else
        Cpu_clearFlag(cpu, N);
    
    if (cpu->y == 0)
        Cpu_setFlag(cpu, Z);
    else 
        Cpu_clearFlag(cpu, Z);
}

void INC(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("INC ");
    #endif
    unsigned short addr = fetchAddr(cpu, addr_mode);
    cpu->ram[addr] += 1;
    
    //setting Z and N flags
    if (cpu->ram[addr] < 0)
        Cpu_setFlag(cpu, N);
    else
        Cpu_clearFlag(cpu, N);
    
    if (cpu->ram[addr] == 0)
        Cpu_setFlag(cpu, Z);
    else 
        Cpu_clearFlag(cpu, Z);

    //timing adjustments
    if (addr_mode == absx)
        cpu->cycleCounter = 7;
    else
        cpu->cycleCounter += 2;
}

void INX(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("INX ");
    #endif
    fetchOperand(cpu, addr_mode);
    cpu->x += 1;
    
    //setting Z and N flags
    if (cpu->x < 0)
        Cpu_setFlag(cpu, N);
    else
        Cpu_clearFlag(cpu, N);
    
    if (cpu->x == 0)
        Cpu_setFlag(cpu, Z);
    else 
        Cpu_clearFlag(cpu, Z);
}

void INY(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("INY ");
    #endif
    fetchOperand(cpu, addr_mode);
    cpu->x += 1;
    
    //setting Z and N flags
    if (cpu->y < 0)
        Cpu_setFlag(cpu, N);
    else
        Cpu_clearFlag(cpu, N);
    
    if (cpu->y == 0)
        Cpu_setFlag(cpu, Z);
    else 
        Cpu_clearFlag(cpu, Z);
}

//arithmetic operations
void ADC(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("ADC ");
    #endif
    unsigned char operand = fetchOperand(cpu, addr_mode);
    unsigned char prev_carry = cpu->p & 0x01;
    unsigned short result = (unsigned short) cpu->a + (unsigned short) operand + (unsigned short) prev_carry;

    //setting carry flag
    cpu->p &= 0xFE;
    if ((result & 0x0100) == 0x0100)
        cpu->p |= 0x01;

    //saving result on acc
    result &= 0x00FF;
    cpu->a = (unsigned char) result;

    //setting Z and N flags
    if (cpu->a < 0)
        Cpu_setFlag(cpu, N);
    else
        Cpu_clearFlag(cpu, N);
    if (cpu->a == 0)
        Cpu_setFlag(cpu, Z);
    else 
        Cpu_clearFlag(cpu, Z);
    
    //setting V flag
    if ((cpu->a ^ result) & (operand ^ result) & 0x80)
        Cpu_setFlag(cpu, V);
}

void SBC(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("SBC ");
    #endif
    unsigned char operand = fetchOperand(cpu, addr_mode);
    operand = ~operand;
    unsigned char prev_carry = cpu->p & 0x01;
    unsigned short result = (unsigned short) cpu->a + (unsigned short) operand + (unsigned short) prev_carry;

    //setting carry flag
    cpu->p &= 0xFE;
    if ((result & 0x0100) == 0x0100)
        cpu->p |= 0x01;

    //saving result on acc
    result &= 0x00FF;
    cpu->a = (unsigned char) result;

    //setting Z and N flags
    if (cpu->a < 0)
        Cpu_setFlag(cpu, N);
    else
        Cpu_clearFlag(cpu, N);
    if (cpu->a == 0)
        Cpu_setFlag(cpu, Z);
    else 
        Cpu_clearFlag(cpu, Z);
    
    //setting V flag
    if ((cpu->a ^ result) & (operand ^ result) & 0x80)
        Cpu_setFlag(cpu, V);
}

//logical operations
void AND(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("AND ");
    #endif
    unsigned char operand = fetchOperand(cpu, addr_mode);
    cpu->a &= operand;

    //setting Z and N flags
    if (cpu->a < 0)
        Cpu_setFlag(cpu, N);
    else
        Cpu_clearFlag(cpu, N);
    if (cpu->a == 0)
        Cpu_setFlag(cpu, Z);
    else 
        Cpu_clearFlag(cpu, Z);
}

void EOR(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("EOR ");
    #endif
    unsigned char operand = fetchOperand(cpu, addr_mode);
    cpu->a ^= operand;

    //setting Z and N flags
    if (cpu->a < 0)
        Cpu_setFlag(cpu, N);
    else
        Cpu_clearFlag(cpu, N);
    if (cpu->a == 0)
        Cpu_setFlag(cpu, Z);
    else 
        Cpu_clearFlag(cpu, Z);
}

void ORA(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("ORA ");
    #endif
    unsigned char operand = fetchOperand(cpu, addr_mode);
    cpu->a |= operand;

    //setting Z and N flags
    if (cpu->a < 0)
        Cpu_setFlag(cpu, N);
    else
        Cpu_clearFlag(cpu, N);
    if (cpu->a == 0)
        Cpu_setFlag(cpu, Z);
    else 
        Cpu_clearFlag(cpu, Z);
}

//shift & rotate instructions
void ASL(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("ASL ");
    #endif
    unsigned char shifted_bit;
    if (addr_mode == acc) {
        fetchOperand(cpu, addr_mode);
        shifted_bit = cpu->a & 0x80;

        cpu->a = cpu->a << 1;

        //setting C flag
        if (shifted_bit == 0x80)
            Cpu_setFlag(cpu, C);
        else
            Cpu_clearFlag(cpu, C);
        
        //setting Z and N flags
        if (cpu->a < 0)
            Cpu_setFlag(cpu, N);
        else
            Cpu_clearFlag(cpu, N);
        if (cpu->a == 0)
            Cpu_setFlag(cpu, Z);
        else 
            Cpu_clearFlag(cpu, Z);
        }
    else {
        unsigned short addr = fetchAddr(cpu, addr_mode);
        shifted_bit = cpu->ram[addr] & 0x80;

        cpu->ram[addr] = cpu->ram[addr] << 1;

        //setting C flag
        if (shifted_bit == 0x80)
            Cpu_setFlag(cpu, C);
        else
            Cpu_clearFlag(cpu, C);
        
        //setting Z and N flags
        if (cpu->ram[addr] < 0)
            Cpu_setFlag(cpu, N);
        else
            Cpu_clearFlag(cpu, N);
        if (cpu->ram[addr] == 0)
            Cpu_setFlag(cpu, Z);
        else 
            Cpu_clearFlag(cpu, Z);

        //timing adjustments
        if (addr_mode == absx)
            cpu->cycleCounter = 7;
        else
            cpu->cycleCounter += 2;
    }
}

void LSR(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("LSR ");
    #endif
    unsigned char shifted_bit;
    if (addr_mode == acc) {
        fetchOperand(cpu, addr_mode);
        shifted_bit = cpu->a & 0x01;

        cpu->a = cpu->a >> 1;

        //setting C flag
        if (shifted_bit == 0x01)
            Cpu_setFlag(cpu, C);
        else
            Cpu_clearFlag(cpu, C);
        
        //setting Z and N flags
        if (cpu->a < 0)
            Cpu_setFlag(cpu, N);
        else
            Cpu_clearFlag(cpu, N);
        if (cpu->a == 0)
            Cpu_setFlag(cpu, Z);
        else 
            Cpu_clearFlag(cpu, Z);
        }
    else {
        unsigned short addr = fetchAddr(cpu, addr_mode);
        shifted_bit = cpu->ram[addr] & 0x01;

        cpu->ram[addr] = cpu->ram[addr] << 1;

        //setting C flag
        if (shifted_bit == 0x01)
            Cpu_setFlag(cpu, C);
        else
            Cpu_clearFlag(cpu, C);
        
        //setting Z and N flags
        if (cpu->ram[addr] < 0)
            Cpu_setFlag(cpu, N);
        else
            Cpu_clearFlag(cpu, N);
        if (cpu->ram[addr] == 0)
            Cpu_setFlag(cpu, Z);
        else 
            Cpu_clearFlag(cpu, Z);

        //timing adjustments
        if (addr_mode == absx)
            cpu->cycleCounter = 7;
        else
            cpu->cycleCounter += 2;
    }
}

void ROL(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("ROL ");
    #endif
    unsigned char shifted_bit;
    if (addr_mode == acc) {
        fetchOperand(cpu, addr_mode);
        shifted_bit = cpu->a & 0x80;
        shifted_bit = shifted_bit >> 7;

        cpu->a = cpu->a << 1;
        cpu->a |= shifted_bit;

        //setting C flag
        if (shifted_bit == 0x01)
            Cpu_setFlag(cpu, C);
        else
            Cpu_clearFlag(cpu, C);
        
        //setting Z and N flags
        if (cpu->a < 0)
            Cpu_setFlag(cpu, N);
        else
            Cpu_clearFlag(cpu, N);
        if (cpu->a == 0)
            Cpu_setFlag(cpu, Z);
        else 
            Cpu_clearFlag(cpu, Z);
        }
    else {
        unsigned short addr = fetchAddr(cpu, addr_mode);

        shifted_bit = cpu->ram[addr] & 0x80;
        shifted_bit = shifted_bit >> 7;

        cpu->ram[addr] = cpu->ram[addr] << 1;
        cpu->ram[addr] |= shifted_bit;

        //setting C flag
        if (shifted_bit == 0x01)
            Cpu_setFlag(cpu, C);
        else
            Cpu_clearFlag(cpu, C);
        
        //setting Z and N flags
        if (cpu->ram[addr] < 0)
            Cpu_setFlag(cpu, N);
        else
            Cpu_clearFlag(cpu, N);
        if (cpu->ram[addr] == 0)
            Cpu_setFlag(cpu, Z);
        else 
            Cpu_clearFlag(cpu, Z);

        //timing adjustments
        if (addr_mode == absx)
            cpu->cycleCounter = 7;
        else
            cpu->cycleCounter += 2;
    }
}

void ROR(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("ROR ");
    #endif
    unsigned char shifted_bit;
    if (addr_mode == acc) {
        fetchOperand(cpu, addr_mode);
        shifted_bit = cpu->a & 0x01;
        shifted_bit = shifted_bit << 7;

        cpu->a = cpu->a >> 1;
        cpu->a |= shifted_bit;

        //setting C flag
        if (shifted_bit == 0x80)
            Cpu_setFlag(cpu, C);
        else
            Cpu_clearFlag(cpu, C);
        
        //setting Z and N flags
        if (cpu->a < 0)
            Cpu_setFlag(cpu, N);
        else
            Cpu_clearFlag(cpu, N);
        if (cpu->a == 0)
            Cpu_setFlag(cpu, Z);
        else 
            Cpu_clearFlag(cpu, Z);
        }
    else {
        unsigned short addr = fetchAddr(cpu, addr_mode);

        shifted_bit = cpu->ram[addr] & 0x01;
        shifted_bit = shifted_bit << 7;

        cpu->ram[addr] = cpu->ram[addr] >> 1;
        cpu->ram[addr] |= shifted_bit;

        //setting C flag
        if (shifted_bit == 0x80)
            Cpu_setFlag(cpu, C);
        else
            Cpu_clearFlag(cpu, C);
        
        //setting Z and N flags
        if (cpu->ram[addr] < 0)
            Cpu_setFlag(cpu, N);
        else
            Cpu_clearFlag(cpu, N);
        if (cpu->ram[addr] == 0)
            Cpu_setFlag(cpu, Z);
        else 
            Cpu_clearFlag(cpu, Z);

        //timing adjustments
        if (addr_mode == absx)
            cpu->cycleCounter = 7;
        else
            cpu->cycleCounter += 2;
    }
}

//flag instructions
void CLC(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("CLC ");
    #endif
    fetchOperand(cpu, addr_mode);
    Cpu_clearFlag(cpu, C);
}

void CLD(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("CLD ");
    #endif
    fetchOperand(cpu, addr_mode);
    Cpu_clearFlag(cpu, D);
}

void CLI(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("CLI ");
    #endif
    fetchOperand(cpu, addr_mode);
    Cpu_clearFlag(cpu, I);
}

void CLV(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("CLV ");
    #endif
    fetchOperand(cpu, addr_mode);
    Cpu_clearFlag(cpu, V);
}

void SEC(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("SEC ");
    #endif
    fetchOperand(cpu, addr_mode);
    Cpu_setFlag(cpu, C);
}

void SED(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("SED ");
    #endif
    fetchOperand(cpu, addr_mode);
    Cpu_setFlag(cpu, D);
}

void SEI(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("SEI ");
    #endif
    fetchOperand(cpu, addr_mode);
    Cpu_setFlag(cpu, I);
}

//comparisons
void CMP(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("CMP ");
    #endif
    unsigned short addr = fetchAddr(cpu, addr_mode);

    if (cpu->a < cpu->ram[addr]) {
        Cpu_clearFlag(cpu, Z);
        Cpu_clearFlag(cpu, C);
        Cpu_setFlag(cpu, N);
    }
    else if (cpu->a == cpu->ram[addr]) {
        Cpu_setFlag(cpu, Z);
        Cpu_setFlag(cpu, C);
        Cpu_clearFlag(cpu, N);
    }
    else { //reg > op
        Cpu_clearFlag(cpu, Z);
        Cpu_setFlag(cpu, C);
        Cpu_clearFlag(cpu, N);
    }
}

void CPX(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("CPX ");
    #endif
    unsigned short addr = fetchAddr(cpu, addr_mode);

    if (cpu->x < cpu->ram[addr]) {
        Cpu_clearFlag(cpu, Z);
        Cpu_clearFlag(cpu, C);
        Cpu_setFlag(cpu, N);
    }
    else if (cpu->x == cpu->ram[addr]) {
        Cpu_setFlag(cpu, Z);
        Cpu_setFlag(cpu, C);
        Cpu_clearFlag(cpu, N);
    }
    else { //reg > op
        Cpu_clearFlag(cpu, Z);
        Cpu_setFlag(cpu, C);
        Cpu_clearFlag(cpu, N);
    }
}

void CPY(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("CPY ");
    #endif
    unsigned short addr = fetchAddr(cpu, addr_mode);

    if (cpu->y < cpu->ram[addr]) {
        Cpu_clearFlag(cpu, Z);
        Cpu_clearFlag(cpu, C);
        Cpu_setFlag(cpu, N);
    }
    else if (cpu->y == cpu->ram[addr]) {
        Cpu_setFlag(cpu, Z);
        Cpu_setFlag(cpu, C);
        Cpu_clearFlag(cpu, N);
    }
    else { //reg > op
        Cpu_clearFlag(cpu, Z);
        Cpu_setFlag(cpu, C);
        Cpu_clearFlag(cpu, N);
    }
}

//conditional branch instructions
void BCC(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("BCC ");
    #endif

    unsigned short branch_addr = fetchAddr(cpu, addr_mode);
    if ((cpu->p & 0x01) == 0x00) {
        cpu->pc = branch_addr;
    }
}

void BCS(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("BCS ");
    #endif

    unsigned short branch_addr = fetchAddr(cpu, addr_mode);
    if ((cpu->p & 0x01) == 0x01) {
        cpu->pc = branch_addr;
    }
}

void BEQ(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("BEQ ");
    #endif

    unsigned short branch_addr = fetchAddr(cpu, addr_mode);
    if ((cpu->p & 0x02) == 0x02) {
        cpu->pc = branch_addr;
    }
}

void BNE(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("BNE ");
    #endif

    unsigned short branch_addr = fetchAddr(cpu, addr_mode);
    if ((cpu->p & 0x02) == 0x00) {
        cpu->pc = branch_addr;
    }
}

void BMI(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("BMI ");
    #endif

    unsigned short branch_addr = fetchAddr(cpu, addr_mode);
    if ((cpu->p & 0x80) == 0x80) {
        cpu->pc = branch_addr;
    }
}

void BPL(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("BPL ");
    #endif

    unsigned short branch_addr = fetchAddr(cpu, addr_mode);
    if ((cpu->p & 0x80) == 0x00) {
        cpu->pc = branch_addr;
    }
}

void BVC(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("BVC ");
    #endif

    unsigned short branch_addr = fetchAddr(cpu, addr_mode);
    if ((cpu->p & 0x40) == 0x00) {
        cpu->pc = branch_addr;
    }
}

void BVS(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("BVS ");
    #endif

    unsigned short branch_addr = fetchAddr(cpu, addr_mode);
    if ((cpu->p & 0x40) == 0x40) {
        cpu->pc = branch_addr;
    }
}

//jumps & subroutines
void JMP(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("JMP ");
    #endif
    unsigned short addr = fetchAddr(cpu, addr_mode);
    cpu->pc = addr;

    //timing adjustments
    cpu->cycleCounter -= 1;
}

void JSR(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("JSR ");
    #endif
    unsigned short jmp_addr = fetchAddr(cpu, addr_mode);

    unsigned short curr_addr = cpu->pc;
    unsigned char curr_addr_l = curr_addr & 0x00FF;
    curr_addr = curr_addr >> 8;
    unsigned char curr_addr_h = curr_addr & 0x00FF;
    
    pushStack(cpu, curr_addr_h);
    pushStack(cpu, curr_addr_l);

    cpu->pc = jmp_addr;

    //timing adjustments
    cpu->cycleCounter += 2;
}

void RTS(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("RTS ");
    #endif
    fetchOperand(cpu, addr_mode);
    unsigned char ret_addr_l = popStack(cpu);
    unsigned char ret_addr_h = popStack(cpu);

    unsigned short ret_addr = ret_addr_h;
    ret_addr = ret_addr << 8;
    ret_addr |= ret_addr_l;
    
    cpu->pc = ret_addr;

    //timing adjustments
    cpu->cycleCounter += 4;
}

//interrupts
void BRK(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("BRK ");
    #endif
    fetchOperand(cpu, addr_mode);
    unsigned short ret_addr = cpu->pc + 2;

    unsigned char ret_addr_l = ret_addr & 0x00FF;
    ret_addr = ret_addr >> 8;
    unsigned char ret_addr_h = ret_addr & 0x00FF;
    
    cpu->p |= 1 >> B;

    pushStack(cpu, ret_addr_h);
    pushStack(cpu, ret_addr_l);
    pushStack(cpu, cpu->p);
    
    cpu->p |= 1 >> I;

    
    unsigned char pointer_ll = cpu->ram[IRQ_START];
    unsigned char pointer_hh = cpu->ram[IRQ_START + 1];
    unsigned short pointer = 0 | pointer_hh;  
    pointer = pointer << 8;
    pointer |= pointer_ll;
    cpu->pc = cpu->ram[pointer];

    //timing adjustments
    cpu->cycleCounter += 5;
}


void RTI(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("RTI ");
    #endif
    fetchOperand(cpu, addr_mode);
    cpu->p = popStack(cpu);
    unsigned char ret_addr_l = popStack(cpu);
    unsigned char ret_addr_h = popStack(cpu);

    unsigned short ret_addr = ret_addr_h;
    ret_addr = ret_addr << 8;
    ret_addr |= ret_addr_l;
    
    cpu->pc = ret_addr;

    //timing adjustments
    cpu->cycleCounter += 4;
}

//other

void BIT(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("BIT ");
    #endif
    unsigned char operand = fetchOperand(cpu, addr_mode);
    unsigned char operand_n_v = operand & 0xC0;

    cpu->p |= operand_n_v;


    //setting Z flag
    if ((operand & cpu->a) != 0)
        Cpu_setFlag(cpu, Z);
    else
        Cpu_clearFlag(cpu, Z);
}

void NOP(Cpu *cpu, int addr_mode) {
    #ifdef DEBUG
    printf("NOP ");
    #endif
    fetchOperand(cpu, addr_mode);
}

void XXX(){}