#include "arch/avr/faults.hh"
#include "arch/avr/pcstate.hh"
#include "arch/avr/types.hh"
#include "cpu/thread_context.hh"
#include "sim/core.hh"
#include "sim/cur_tick.hh"
#include "sim/sim_exit.hh"
#include "sim/system.hh"
#include <iostream>

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
  // Halt completely on an unimplemented/undecoded instruction. Skipping it
  // (the old behaviour) silently corrupts results and invalidates any
  // workload measurement, so stop hard and name the opcode + PC so it can be
  // implemented in src/arch/avr/isa/decoder/ before re-running.
  panic("AVR: unimplemented/undecoded instruction opcode=0x%08x at PC=0x%lx "
        "-- halting (implement it in src/arch/avr, then rebuild).",
        (uint32_t)_opcode, (uint64_t)tc->pcState().instAddr());
}

// HaltFault is defined inline in faults.hh — nothing extra needed here.

void SyscallFault::invoke(ThreadContext *tc, const StaticInstPtr &inst) {
  // Syscall convention: R16 = syscall number, R17 = argument
  // Syscall 1: putchar(R17)
  // Syscall 2: exit()
  // Syscall 3: get_time()
  RegVal num = tc->getReg(RegId(intRegClass, 16));
  warn("AVR Syscall triggered: num=%d", (int)num);
  if (num == 1) {
    RegVal arg = tc->getReg(RegId(intRegClass, 17));
    std::cout << (char)arg << std::flush;
  } else if (num == 2) {
    gem5::exitSimLoop("AVR Syscall Exit");
  } else if (num == 3) {
    // get_time: return milliseconds in R22-R25
    uint32_t ms = curTick() / gem5::sim_clock::as_int::ms;
    tc->setReg(RegId(intRegClass, 22), (ms & 0xFF));
    tc->setReg(RegId(intRegClass, 23), ((ms >> 8) & 0xFF));
    tc->setReg(RegId(intRegClass, 24), ((ms >> 16) & 0xFF));
    tc->setReg(RegId(intRegClass, 25), ((ms >> 24) & 0xFF));
  } else {
    warn("Unknown AVR syscall number: %d", (int)num);
  }

  // Advance PC to next instruction
  auto pc = tc->pcState().as<PCState>();
  pc.advance();
  tc->pcState(pc);
}

} // namespace AVRISAInst
} // namespace gem5
