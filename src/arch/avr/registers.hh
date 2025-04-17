#ifndef __ARCH_AVR_REGISTERS_HH__
#define __ARCH_AVR_REGISTERS_HH__

#include "base/types.hh"
#include "cpu/reg_class.hh"

namespace gem5
{

    namespace AVRISAInst
    {

        // Number of general purpose registers
        const int NumGPRegs = 32;

        // Status Register (SREG) bits
        const int SREG_I = 7; // Global Interrupt Enable
        const int SREG_T = 6; // Transfer bit
        const int SREG_H = 5; // Half Carry Flag
        const int SREG_S = 4; // Sign bit
        const int SREG_V = 3; // Overflow Flag
        const int SREG_N = 2; // Negative Flag
        const int SREG_Z = 1; // Zero Flag
        const int SREG_C = 0; // Carry Flag

        // Register indices
        const int PC_Idx = 32;   // Program Counter
        const int SREG_Idx = 33; // Status Register
        const int SP_Idx = 34;   // Stack Pointer

        // Total number of registers
        const int NumRegs = NumGPRegs + 3;
        enum MiscRegIndex
        {
            MISCREG_SREG = 0,
            // ... other misc registers
            NUM_MISCREGS
        };
    } // namespace AVRISAInst
} // namespace gem5

#endif // __ARCH_AVR_REGISTERS_HH__
