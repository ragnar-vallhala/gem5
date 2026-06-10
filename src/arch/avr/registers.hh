#ifndef __ARCH_AVR_REGISTERS_HH__
#define __ARCH_AVR_REGISTERS_HH__

#include "base/types.hh"
#include "cpu/reg_class.hh"

namespace gem5 {

namespace AVRISAInst {
extern const RegClass intRegClass;
extern const RegClass floatRegClass;
extern const RegClass vecRegClass;
extern const RegClass vecElemClass;
extern const RegClass vecPredRegClass;
extern const RegClass matRegClass;
extern const RegClass ccRegClass;
extern const RegClass miscRegClass;

// Number of general purpose registers
const int NumGPRegs = 32;

// General Purpose Register (GPR) indices
enum GPRIndex {
  R0_Idx = 0,
  R1_Idx = 1,
  R2_Idx = 2,
  R3_Idx = 3,
  R4_Idx = 4,
  R5_Idx = 5,
  R6_Idx = 6,
  R7_Idx = 7,
  R8_Idx = 8,
  R9_Idx = 9,
  R10_Idx = 10,
  R11_Idx = 11,
  R12_Idx = 12,
  R13_Idx = 13,
  R14_Idx = 14,
  R15_Idx = 15,
  R16_Idx = 16,
  R17_Idx = 17,
  R18_Idx = 18,
  R19_Idx = 19,
  R20_Idx = 20,
  R21_Idx = 21,
  R22_Idx = 22,
  R23_Idx = 23,
  R24_Idx = 24,
  R25_Idx = 25,
  R26_Idx = 26, // X-register low byte
  R27_Idx = 27, // X-register high byte
  R28_Idx = 28, // Y-register low byte
  R29_Idx = 29, // Y-register high byte
  R30_Idx = 30, // Z-register low byte
  R31_Idx = 31  // Z-register high byte
};

// Special register pairs
const int X_REG_PAIR = R26_Idx; // R27:R26
const int Y_REG_PAIR = R28_Idx; // R29:R28
const int Z_REG_PAIR = R30_Idx; // R31:R30

// Status Register (SREG) bits
const int SREG_I = 7; // Global Interrupt Enable
const int SREG_T = 6; // Transfer bit
const int SREG_H = 5; // Half Carry Flag
const int SREG_S = 4; // Sign bit
const int SREG_V = 3; // Overflow Flag
const int SREG_N = 2; // Negative Flag
const int SREG_Z = 1; // Zero Flag
const int SREG_C = 0; // Carry Flag

// Total number of integer registers.
// Only the 32 GPRs live in the integer register class. PC is modeled via
// PCState; SREG and SP are MiscRegs (MISCREG_SREG / MISCREG_SP) and are the
// single source of truth for those values.
const int NumRegs = NumGPRegs;

// Misc Register Indices
enum MiscRegIndex {
  MISCREG_SREG = 0,
  MISCREG_SP,    // Stack Pointer
  MISCREG_RAMPX, // Extended X-register Address
  MISCREG_RAMPY, // Extended Y-register Address
  MISCREG_RAMPZ, // Extended Z-register Address
  MISCREG_EIND,  // Extended Indirect Jump/Call
  MISCREG_SPL,   // Stack Pointer Low
  MISCREG_SPH,   // Stack Pointer High
  NUM_MISCREGS
};

// Register name strings for debugging/tracing
const char *const RegisterNames[] = {
    "r0",  "r1",  "r2",  "r3",  "r4",  "r5",  "r6",   "r7",  "r8",
    "r9",  "r10", "r11", "r12", "r13", "r14", "r15",  "r16", "r17",
    "r18", "r19", "r20", "r21", "r22", "r23", "r24",  "r25", "r26",
    "r27", "r28", "r29", "r30", "r31", "pc",  "sreg", "sp"};

// Register aliases
const char *const RegisterAliases[] = {
    "", "", "", "", "", "", "", "", "", "", "",   "",   "",   "",   "",   "",
    "", "", "", "", "", "", "", "", "", "", "XL", "XH", "YL", "YH", "ZL", "ZH"};

} // namespace AVRISAInst
} // namespace gem5

#endif // __ARCH_AVR_REGISTERS_HH__
