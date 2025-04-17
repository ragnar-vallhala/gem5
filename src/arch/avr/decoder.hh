#ifndef __ARCH_AVR_DECODER_HH__
#define __ARCH_AVR_DECODER_HH__

#include "arch/avr/types.hh"
#include "cpu/static_inst.hh"

namespace gem5
{

namespace AVRISAInst {

class Decoder
{
  public:
    Decoder() {}

    StaticInstPtr decodeInst(ExtMachInst mach_inst);
};

} // namespace AVRISAInst
} // namespace gem5

#endif // __ARCH_AVR_DECODER_HH__
