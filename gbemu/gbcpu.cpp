#include "gb.h"
#include <tchar.h>
#include <stdio.h>
#include <Windows.h>

static bool run = true;
static bool interrupts = true;

#define NOP(opcode) case opcode: { cpu.PC += 1; break; }

#define LD_8(opcode, regA, regB) case opcode: { cpu.regA = cpu.regB; cpu.PC += 1; break; }
#define LD_8M(opcode, regA) case opcode: { cpu.regA = readByte(gb, cpu.HL); cpu.PC += 1; break; }
#define LD_M8(opcode, regB) case opcode: { writeByte(gb, readShortArg(), cpu.regB); cpu.PC += 2; break; }

#define LD_BLK(base, reg)   \
    LD_8(base + 0, reg, B)  \
    LD_8(base + 1, reg, C)  \
    LD_8(base + 2, reg, D)  \
    LD_8(base + 3, reg, E)  \
    LD_8(base + 4, reg, H)  \
    LD_8(base + 5, reg, L)  \
    LD_8M(base + 6, reg)    \
    LD_8(base + 7, reg, A)  \

#define HALT(opcode) case opcode: { halt(gb); cpu.PC += 1; break; }
#define LD_HL(opcode, regB) case opcode: { writeByte(gb, cpu.HL, cpu.regB); cpu.PC += 1; break; }

#define LD_HL_BLK(base) \
    LD_HL(base + 0, B)  \
    LD_HL(base + 1, C)  \
    LD_HL(base + 2, D)  \
    LD_HL(base + 3, E)  \
    LD_HL(base + 4, H)  \
    LD_HL(base + 5, L)  \
    HALT (base + 6)     \
    LD_HL(base + 7, A)

#define JR_COND_C   (cpu.F & CARRY_FLAG)
#define JR_COND_Z   (cpu.F & ZERO_FLAG)
#define JR_COND_NC (JR_COND_C == 0)
#define JR_COND_NZ (JR_COND_Z == 0)
#define JR_COND_NONE (true)

#define JR_8(opcode, cond) case opcode: {   \
    if (cond) {                             \
        cpu.PC += (char) readByteArg();    \
    }                                       \
    cpu.PC += 2;                            \
    break;                                  \
}

#define ADD_8(opcode, bVal, inc) case opcode: {             \
    cpu.F &= ~ALL_FLAGS;                                    \
    UINT8 b = bVal;                                         \
    UINT8 lowerRes = (cpu.A & 0xf) + (b & 0xf);             \
    UINT16 highRes = (cpu.A & 0xf0) + (b & 0xf0);           \
    if (lowerRes > 0xF) cpu.F |= HALF_CARRY_FLAG;           \
    UINT32 res = highRes + lowerRes;                        \
    if (res > 0xFF) cpu.F |= CARRY_FLAG;                    \
    cpu.A = res & 0xFF;                                     \
    cpu.F &= ~SUBTRACT_FLAG;                                \
    cpu.PC += inc;                                          \
    break;                                                  \
}

#define ADC_8(opcode, bVal, inc) case opcode: {             \
    cpu.F &= ~ALL_FLAGS;                                    \
    UINT8 cy = (cpu.F & CARRY_FLAG) >> 3;                   \
    UINT8 b = bVal;                                         \
    UINT8 lowerRes = (cpu.A & 0xf) + (b & 0xf) + cy;        \
    UINT16 highRes = (cpu.A & 0xf0) + (b & 0xf0);           \
    if (lowerRes > 0xF) cpu.F |= HALF_CARRY_FLAG;           \
    UINT32 res = highRes + lowerRes;                        \
    if (res > 0xFF) cpu.F |= CARRY_FLAG;                    \
    cpu.A = res & 0xFF;                                     \
    cpu.F &= ~SUBTRACT_FLAG;                                \
    cpu.PC += inc;                                          \
    break;                                                  \
}

#define SUB_8(opcode, bVal, inc) case opcode: {                    \
    cpu.F &= ~ALL_FLAGS;                                    \
    UINT8 b = bVal;                                         \
    UINT8 lowerRes = (cpu.A & 0xf) - (b & 0xf);             \
    UINT16 highRes = (cpu.A & 0xf0) - (b & 0xf0);           \
    if ((cpu.A & 0xf) < (b & 0xf)) cpu.F |= HALF_CARRY_FLAG;\
    UINT32 res = highRes + lowerRes;                        \
    if (cpu.A < b) cpu.F |= CARRY_FLAG;                     \
    cpu.A = res & 0xFF;                                     \
    cpu.F |= SUBTRACT_FLAG;                                 \
    cpu.PC += inc;                                          \
    break;                                                  \
}

#define CP_8(opcode, bVal, inc) case opcode: {                    \
    cpu.F &= ~ALL_FLAGS;                                    \
    UINT8 b = bVal;                                         \
    UINT8 lowerRes = (cpu.A & 0xf) - (b & 0xf);             \
    UINT16 highRes = (cpu.A & 0xf0) - (b & 0xf0);           \
    if ((cpu.A & 0xf) < (b & 0xf)) cpu.F |= HALF_CARRY_FLAG;\
    UINT32 res = highRes + lowerRes;                        \
    if (cpu.A < b) cpu.F |= CARRY_FLAG;                     \
    cpu.F |= SUBTRACT_FLAG;                                 \
    cpu.PC += inc;                                          \
    break;                                                  \
}

#define SBC_8(opcode, bVal, inc) case opcode: {                    \
    cpu.F &= ~ALL_FLAGS;                                    \
    UINT8 cy = (cpu.F & CARRY_FLAG) >> 3;                   \
    UINT8 b = bVal + cy;                                    \
    UINT8 lowerRes = (cpu.A & 0xf) - (b & 0xf);             \
    UINT16 highRes = (cpu.A & 0xf0) - (b & 0xf0);           \
    if ((cpu.A & 0xf) < (b & 0xf)) cpu.F |= HALF_CARRY_FLAG;\
    UINT32 res = highRes + lowerRes;                        \
    if (cpu.A < b) cpu.F |= CARRY_FLAG;                     \
    cpu.A = res & 0xFF;                                     \
    cpu.F |= SUBTRACT_FLAG;                                 \
    cpu.PC += inc;                                          \
    break;                                                  \
}

#define AND_8(opcode, bVal, inc) case opcode: {                    \
    cpu.A &= bVal;                                             \
    cpu.F &= ~ALL_FLAGS;                                    \
    cpu.F |= HALF_CARRY_FLAG;                               \
    if (cpu.A == 0) cpu.F |= ZERO_FLAG;                     \
    cpu.PC += inc;                                          \
    break;                                                  \
}

#define XOR_8(opcode, bVal, inc) case opcode: {                     \
    cpu.A ^= bVal;                                             \
    cpu.F &= ~ALL_FLAGS;                                    \
    if (cpu.A == 0) cpu.F |= ZERO_FLAG;                     \
    cpu.PC += inc;                                          \
    break;                                                  \
}

#define OR_8(opcode, bVal, inc) case opcode: {                     \
    cpu.A |= bVal;                                             \
    cpu.F &= ~ALL_FLAGS;                                    \
    if (cpu.A == 0) cpu.F |= ZERO_FLAG;                     \
    cpu.PC += inc;                                          \
    break;                                                  \
}

#define BLK(base, op, inc)                   \
    op(base + 0, cpu.B, inc)                 \
    op(base + 1, cpu.C, inc)                 \
    op(base + 2, cpu.D, inc)                 \
    op(base + 3, cpu.E, inc)                 \
    op(base + 4, cpu.H, inc)                 \
    op(base + 5, cpu.L, inc)                 \
    op(base + 6, readByte(gb, cpu.HL), inc)  \
    op(base + 7, cpu.A, inc)

#define JP_A16(opcode) case opcode: { cpu.PC = readShortArg(); break; }

#define readShortArg() readShort(gb, cpu.PC + 1)
#define readByteArg() readByte(gb, cpu.PC + 1)

static void halt(Gb * gb) {
    OutputDebugString(_T("HALT"));
    exit(0x20);
}

static void instr(Gb* gb)
{
    gbCpu &cpu = gb->cpu;
    BYTE instr = readByte(gb, cpu.PC);

    debugPrint("PC: %x OP: %x\n", cpu.PC, instr);

    switch (instr) {
        NOP(0x00)
        JR_8(0x18, JR_COND_NONE)
        JR_8(0x20, JR_COND_NZ)
        JR_8(0x28, JR_COND_Z)
        JR_8(0x30, JR_COND_NC)
        JR_8(0x38, JR_COND_C)
        LD_BLK(0x40, B)
        LD_BLK(0x48, C)
        LD_BLK(0x50, D)
        LD_BLK(0x58, E)
        LD_BLK(0x60, H)
        LD_BLK(0x68, L)
        LD_HL_BLK(0x70)
        LD_BLK(0x78, A)

        BLK(0x80, ADD_8, 1)
        BLK(0x88, ADC_8, 1)
        BLK(0x90, SUB_8, 1)
        BLK(0x98, SBC_8, 1)
        BLK(0xA0, AND_8, 1)
        BLK(0xA8, XOR_8, 1)
        BLK(0xB0, OR_8, 1)
        BLK(0xB8, CP_8, 1)

        JP_A16(0xC3)
        LD_M8(0xEA, A)
        CP_8(0xFE, readByteArg(), 2)
    default:
        debugPrint("Unknown OP: %x SP: %x PC: %x\n", instr, cpu.SP, cpu.PC);
        exit(0x19);
    }
}

static void loadAddr(Gb* gb, char* a, char* b)
{

}

static void enableInterrupts(bool newVal)
{
    interrupts = newVal;
}

void execute(Gb* gb)
{
    while (true) {
        instr(gb);
    }
}