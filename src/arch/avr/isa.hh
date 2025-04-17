#ifndef __ARCH_AVR_ISA_HH__
#define __ARCH_AVR_ISA_HH__

#include "arch/avr/registers.hh"
#include "arch/avr/types.hh"
#include "sim/sim_object.hh"

namespace gem5
{

class AVR : public SimObject
{
  public:
    typedef AVRISAInst::MachInst MachInst;
    typedef AVRISAInst::ExtMachInst ExtMachInst;

    AVR(const Params &p);

    void clear();
};

} // namespace gem5

#endif // __ARCH_AVR_ISA_HH__
