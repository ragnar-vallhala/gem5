#ifndef __ARCH_AVR_TYPES_HH__
#define __ARCH_AVR_TYPES_HH__

#include "base/types.hh"

namespace gem5
{

    namespace AVRISAInst
    {
        // AVR instructions are 16 or 32 bits
        typedef uint16_t MachInst;
    }

    namespace AvrISA
    {
        typedef uint8_t RegIndex;
        typedef uint32_t PCState;
        typedef uint16_t IntReg;
        typedef uint8_t CCReg; // Status Register (SREG)
    }

} // namespace gem5

#endif // __ARCH_AVR_TYPES_HH__
