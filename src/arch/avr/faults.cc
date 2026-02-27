#include "arch/avr/faults.hh"
#include "arch/avr/pcstate.hh"
#include "arch/avr/types.hh"
#include "cpu/thread_context.hh"
#include "sim/sim_exit.hh"

namespace gem5 {
namespace AVRISAInst {

// AVRFault base implementation (declared in types.hh)
void AVRFault::invoke(ThreadContext *tc, const StaticInstPtr &inst) {
  panic("AVR fault occurred: %s\n", name());
}

bool AVRFault::returnFromFault(ThreadContext *tc) {
  auto pc = tc->pcState().as<PCState>();
  pc.advance();
  tc->pcState(pc);
  return true;
}

void UnknownInstFault::invoke(ThreadContext *tc, const StaticInstPtr &inst) {
  // Warn and skip the instruction (advance by 2 bytes)
  // to allow the simulator to continue past unimplemented insts.
  warn("Skipping unknown AVR instruction at PC=0x%lx",
       (uint64_t)tc->pcState().instAddr());
  auto &apc = tc->pcState().as<PCState>();
  PCState new_pc = apc;
  new_pc.advance();
  tc->pcState(new_pc);
}

// HaltFault is defined inline in faults.hh — nothing extra needed here.

} // namespace AVRISAInst
} // namespace gem5
