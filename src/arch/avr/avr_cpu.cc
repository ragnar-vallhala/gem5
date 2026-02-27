#include "arch/avr/avr_cpu.hh"
#include "arch/avr/decoder.hh"
#include "arch/avr/faults.hh"
#include "base/logging.hh"
#include "cpu/exec_context.hh"
#include "cpu/simple_thread.hh"
#include "cpu/thread_context.hh"
#include "mem/packet.hh"
#include "params/AVRCPU.hh"
#include "sim/faults.hh"
#include "sim/sim_exit.hh"
#include "sim/system.hh"

namespace gem5 {

AVRCPU::AVRCPU(const AVRCPUParams &p)
    : BaseCPU(p), tickEvent([this] { tick(); }, name() + ".tickEvent"),
      instPort(name() + ".inst_port", this),
      dataPort(name() + ".data_port", this),
      ArchMMU(dynamic_cast<AVRMMU *>(p.mmu)),
      ArchInterrupts(p.interrupts.empty()
                         ? nullptr
                         : dynamic_cast<AVRInterrupts *>(p.interrupts[0])),
      insts(0), ops(0) {
  if (!ArchInterrupts) {
    warn("AVRCPU: No valid AVRInterrupts object bound to CPU.\n");
  }

  warn("numThreads: %d", numThreads);
  warn("isa size: %d", p.isa.size());
  warn("decoder size: %d", p.decoder.size());
  warn("workload size: %d", p.workload.size());

  for (ThreadID tid = 0; tid < numThreads; tid++) {
    if (p.workload.size() > tid) {
      threadContexts.push_back(new SimpleThread(this, tid, p.system,
                                                p.workload[tid], p.mmu,
                                                p.isa[tid], p.decoder[tid]));
    } else {
      threadContexts.push_back(new SimpleThread(this, tid, p.system, p.mmu,
                                                p.isa[tid], p.decoder[tid]));
    }
  }
}

/********************* CPU EXECUTION ************************/

void AVRCPU::wakeup(ThreadID tid) {
  if (threadContexts[tid]->status() == ThreadContext::Suspended) {
    threadContexts[tid]->activate();
  }
}

void AVRCPU::startup() {
  BaseCPU::startup();
  for (ThreadID tid = 0; tid < numThreads; tid++) {
    if (threadContexts[tid]->status() == ThreadContext::Active) {
      if (!tickEvent.scheduled()) {
        schedule(tickEvent, clockEdge(Cycles(1)));
      }
      break;
    }
  }
}

void AVRCPU::tick() {
  executeInstruction();

  // Schedule next tick if active
  if (threadContexts[0]->status() == ThreadContext::Active) {
    schedule(tickEvent, clockEdge(Cycles(1)));
  }
}

void AVRCPU::executeInstruction() {
  ThreadContext *tc = threadContexts[0];

  Addr pc = tc->pcState().instAddr();

  // Fetch 2 bytes from memory (AVR instructions are 16-bit little-endian)
  uint8_t low_byte = system->physProxy.read<uint8_t>(pc);
  uint8_t high_byte = system->physProxy.read<uint8_t>(pc + 1);
  AVRISAInst::ExtMachInst machInst = (high_byte << 8) | low_byte;

  // Check if it's a 32-bit instruction (JMP or CALL)
  // JMP: 1001 010k kkkk 110k (0x940c)
  // CALL: 1001 010k kkkk 111k (0x940e)
  // These share the 1001 010x xxxx 11xx pattern
  if ((machInst & 0xfe0c) == 0x940c) {
    uint8_t low_byte2 = system->physProxy.read<uint8_t>(pc + 2);
    uint8_t high_byte2 = system->physProxy.read<uint8_t>(pc + 3);
    uint32_t second_word = (high_byte2 << 8) | low_byte2;
    machInst = (second_word << 16) | machInst;
  }

  std::cout << "[AVR] PC=" << std::hex << pc << " INSTR=0x" << (int)machInst
            << std::dec << std::endl;

  // Get the AVR-specific decoder
  AVRISAInst::Decoder *decoder =
      dynamic_cast<AVRISAInst::Decoder *>(tc->getDecoderPtr());
  if (!decoder) {
    panic("AVRCPU: Could not cast decoder to AVRISAInst::Decoder");
  }

  // Decode the instruction
  StaticInstPtr inst = decoder->decode(machInst, pc);

  if (!inst) {
    warn("AVRCPU: decode() returned null at PC=0x%x instr=0x%lx", pc, machInst);
    // Advance by 2 bytes to prevent stall
    PCStateBase *pc_state = tc->pcState().clone();
    pc_state->set(pc + 2);
    tc->pcState(*pc_state);
    delete pc_state;
    return;
  }

  // Execute via AVRExecContext wrapper (SimpleThread does not implement
  // ExecContext directly)
  AVRExecContext exec_ctx(static_cast<SimpleThread *>(tc));
  Fault fault = inst->execute(&exec_ctx, nullptr);

  if (fault != NoFault) {
    fault->invoke(tc, inst);
  } else {
    insts++;
    ops++;
    // Advance PC based on the instruction
    PCStateBase *pc_state = tc->pcState().clone();
    inst->advancePC(*pc_state);
    tc->pcState(*pc_state);
    delete pc_state;
  }
}

} // namespace gem5
