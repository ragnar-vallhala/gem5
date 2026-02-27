#ifndef __ARCH_AVR_FAULTS_HH__
#define __ARCH_AVR_FAULTS_HH__

// AVRFault and UnknownInstFault are defined in types.hh.
// This file adds HaltFault for clean simulation exit.
#include "arch/avr/types.hh"
#include "sim/sim_exit.hh"

namespace gem5 {
namespace AVRISAInst {

class HaltFault : public AVRFault {
public:
  HaltFault() : AVRFault("Halt", 99) {}
  const char *name() const override { return "HaltFault"; }
  void invoke(ThreadContext *tc, const StaticInstPtr &inst = nullptr) override {
    warn("AVR SLEEP instruction executed – halting simulation.");
    gem5::exitSimLoop("AVR Halt");
  }
};

} // namespace AVRISAInst
} // namespace gem5

#endif
