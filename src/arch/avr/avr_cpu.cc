#include "arch/avr/avr_cpu.hh"
#include "base/logging.hh"
#include "mem/packet.hh"
#include "params/AVRCPU.hh"
#include "sim/system.hh"

namespace gem5 {

AVRCPU::AVRCPU(const AVRCPUParams &p)
    : BaseCPU(p), instPort(name() + ".inst_port", this),
      dataPort(name() + ".data_port", this),
      ArchMMU(dynamic_cast<AVRMMU *>(p.mmu)),
      ArchInterrupts(p.interrupts.empty()
                         ? nullptr
                         : dynamic_cast<AVRInterrupts *>(p.interrupts[0])),
      tickEvent([this] { tick(); }, name() + ".tickEvent"), insts(0), ops(0) {
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

  // Fetch 1 byte from memory
  uint8_t instr = system->physProxy.read<uint8_t>(pc);

  std::cout << "[AVR] PC=" << std::hex << pc << " INSTR=0x" << (int)instr
            << std::dec << std::endl;

  insts++;
  ops++;

  // For now increment PC by 1 byte
  tc->pcState(pc + 1);
}

} // namespace gem5
