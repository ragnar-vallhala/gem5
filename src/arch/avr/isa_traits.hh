#ifndef __ARCH_AVR_ISA_TRAITS_HH__
#define __ARCH_AVR_ISA_TRAITS_HH__

#include "base/types.hh"

namespace gem5
{

namespace AVRISAInst {

const int MaxInstSrcRegs = 2;    // Maximum number of source registers
const int MaxInstDestRegs = 1;   // Maximum number of destination registers
const int MaxMiscDestRegs = 2;   // Maximum number of misc registers (PC, SREG)

} // namespace AVRISAInst
} // namespace gem5

#endif // __ARCH_AVR_ISA_TRAITS_HH__
