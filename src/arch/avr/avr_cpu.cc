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

#include "base/callback.hh"
#include "base/output.hh"

#include <algorithm>
#include <ostream>
#include <utility>
#include <vector>
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
      insts(0), ops(0), dataWaitStates(p.dataWaitStates) {
  if (!ArchInterrupts) {
    warn("AVRCPU: No valid AVRInterrupts object bound to CPU.\n");
  }

  // Dump the per-mnemonic op-mix histogram when the simulation exits.
  registerExitCallback([this]() { dumpOpMix(); });

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

/********************* MEMORY ACCESS (ATOMIC, VIA PORTS) ************/

void AVRCPU::fetchAtomic(Addr paddr, uint8_t *data, unsigned size) {
  RequestPtr req =
      std::make_shared<Request>(paddr, size, Request::INST_FETCH,
                                instRequestorId());
  Packet pkt(req, MemCmd::ReadReq);
  pkt.dataStatic(data);
  instPort.sendAtomic(&pkt);
}

void AVRCPU::readDataAtomic(Addr paddr, uint8_t *data, unsigned size,
                            Request::Flags flags) {
  RequestPtr req =
      std::make_shared<Request>(paddr, size, flags, dataRequestorId());
  Packet pkt(req, MemCmd::ReadReq);
  pkt.dataStatic(data);
  dataPort.sendAtomic(&pkt);
}

void AVRCPU::writeDataAtomic(Addr paddr, const uint8_t *data, unsigned size,
                             Request::Flags flags) {
  RequestPtr req =
      std::make_shared<Request>(paddr, size, flags, dataRequestorId());
  Packet pkt(req, MemCmd::WriteReq);
  pkt.dataStaticConst(data);
  dataPort.sendAtomic(&pkt);
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
  // Execute one instruction; advance time by its real cycle cost (1-4 cycles)
  // rather than a flat 1, so simulated time reflects AVR instruction timing.
  Cycles cost = executeInstruction();
  baseStats.numCycles += cost;

  // Schedule next tick if active
  if (threadContexts[0]->status() == ThreadContext::Active) {
    schedule(tickEvent, clockEdge(cost));
  }
}

Cycles AVRCPU::executeInstruction() {
  ThreadContext *tc = threadContexts[0];

  Addr pc = tc->pcState().instAddr();

  // Fetch 2 bytes from program memory (AVR instructions are 16-bit
  // little-endian), via the instruction port so the xbar sees fetch traffic.
  uint8_t bytes[2];
  fetchAtomic(pc, bytes, 2);
  AVRISAInst::ExtMachInst machInst = (bytes[1] << 8) | bytes[0];

  // Check if it's a 32-bit instruction
  // JMP/CALL: 1001 010x xxxx 11xx (0x940c/0x940e)
  // LDS: 1001 000d dddd 0000 (0x9000)
  // STS: 1001 001d dddd 0000 (0x9200)
  bool is32 = false;
  if (((machInst & 0xfe0c) == 0x940c) || ((machInst & 0xfe0f) == 0x9000) ||
      ((machInst & 0xfe0f) == 0x9200)) {
    is32 = true;
    uint8_t bytes2[2];
    fetchAtomic(pc + 2, bytes2, 2);
    uint32_t second_word = (bytes2[1] << 8) | bytes2[0];
    machInst = (second_word << 16) | machInst;
  }

  // For Debug
  // std::cout << "[AVR] PC=" << std::hex << pc << " INSTR=0x" << (int)machInst
  //           << std::dec << std::endl;

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
    return Cycles(1);
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
    opHist[inst->getName()]++; // per-mnemonic op-mix
    baseStats.numInsts++;
    baseStats.numOps++;

    // Update thread-level commit stats
    if (!commitStats.empty()) {
      auto &cstats = *commitStats[0];
      cstats.numInsts++;
      cstats.numOps++;
      if (!inst->isMicroop() || inst->isLastMicroop()) {
        cstats.numInstsNotNOP++;
        cstats.numOpsNotNOP++;
      }
      if (inst->isLoad())
        cstats.numLoadInsts++;
      if (inst->isStore())
        cstats.numStoreInsts++;
      if (inst->isControl())
        cstats.updateComCtrlStats(inst);
      if (inst->isInteger())
        cstats.numIntInsts++;
      if (inst->isFloating())
        cstats.numFpInsts++;
    }

    // Update thread-level execute stats
    if (!executeStats.empty()) {
      auto &estats = *executeStats[0];
      estats.numInsts++;
      if (inst->isNop())
        estats.numNop++;
      if (inst->isLoad())
        estats.numLoadInsts++;
      if (inst->isControl())
        estats.numBranches++;
      if (inst->isInteger())
        estats.numIntAluAccesses++;
      if (inst->isFloating())
        estats.numFpAluAccesses++;
      if (inst->isMemRef() || inst->isLoad() || inst->isStore())
        estats.numMemRefs++;
    }

    // Advance PC based on the instruction. NOTE the inline (Basic-format)
    // control instructions follow a "+2 base advance" convention: their
    // execute() leaves PC two bytes short of the intended next PC, and this
    // unconditional advancePC supplies that final +2. Skip instructions
    // (cpse/sbrc/sbrs) do one extra pc.advance() in execute() so the total is
    // +4 (skip the next 2-byte instruction); jump-to-target instructions
    // (icall/ijmp) set target-2 so the +2 lands exactly on target. The
    // template-format control instructions (jmp/call/rcall/ret/rjmp/brbc/brbs)
    // use a no-op advancePC and set the final PC directly in execute().
    PCStateBase *pc_state = tc->pcState().clone();
    inst->advancePC(*pc_state);
    tc->pcState(*pc_state);
    delete pc_state;
  }

  // Cycle cost depends on the instruction and, for branches/skips, on whether
  // control flow was taken (derived from the pc delta).
  return instCycles(machInst, is32, pc, tc->pcState().instAddr());
}

// Cycle counts per the AVR instruction-set manual (AVRe+ core, e.g.
// atmega328p). Most ALU/logic/move ops are 1 cycle (the default); this
// enumerates the multi-cycle ops. Taken conditional branches and successful
// skips cost extra, detected from the pc delta.
Cycles
AVRCPU::instCycles(AVRISAInst::ExtMachInst machInst, bool is32, Addr oldPc,
                   Addr newPc) const
{
  uint16_t op = (uint16_t)machInst; // first 16-bit word holds the opcode

  // ---- 32-bit (two-word) instructions ----
  if (is32) {
    if ((op & 0xfe0e) == 0x940c)
      return Cycles(3); // JMP
    if ((op & 0xfe0e) == 0x940e)
      return Cycles(4); // CALL
    return Cycles(2 + dataWaitStates); // LDS / STS (data memory)
  }

  // ---- single-encoding control / system ops ----
  switch (op) {
  case 0x9508: // RET
  case 0x9518: // RETI
    return Cycles(4);
  case 0x9509: // ICALL
    return Cycles(3);
  case 0x9409: // IJMP
    return Cycles(2);
  case 0x95c8: // LPM r0, Z
  case 0x95d8: // ELPM r0, Z
    return Cycles(3);
  default:
    break;
  }

  // ---- relative jump / call ----
  if ((op & 0xf000) == 0xc000)
    return Cycles(2); // RJMP
  if ((op & 0xf000) == 0xd000)
    return Cycles(3); // RCALL

  // ---- program-memory loads (LPM/ELPM Rd, Z / Z+) ----
  if ((op & 0xfe0e) == 0x9004)
    return Cycles(3);

  // ---- stack ----
  if ((op & 0xfe0f) == 0x920f)
    return Cycles(2 + dataWaitStates); // PUSH (data memory)
  if ((op & 0xfe0f) == 0x900f)
    return Cycles(2 + dataWaitStates); // POP (data memory)

  // ---- data-memory loads/stores ----
  // LD/ST X/Y/Z with post-increment / pre-decrement / displacement, and the
  // 32-bit LDS/STS handled above. Internal SRAM is 2 cycles; dataWaitStates
  // adds a uniform DSE latency penalty (see avr_cpu.hh) when configured.
  if ((op & 0xd000) == 0x8000)
    return Cycles(2 + dataWaitStates); // LDD/STD and LD/ST Y/Z (group 10q0..)
  if ((op & 0xfc00) == 0x9000) {
    uint8_t low = op & 0x000f;
    if (low == 0x1 || low == 0x2 || low == 0x9 || low == 0xa || low == 0xc ||
        low == 0xd || low == 0xe)
      return Cycles(2 + dataWaitStates); // LD/ST X/Y/Z (+/-)
  }

  // ---- multiply family (2 cycles) ----
  if ((op & 0xfc00) == 0x9c00)
    return Cycles(2); // MUL
  if ((op & 0xff00) == 0x0200)
    return Cycles(2); // MULS
  if ((op & 0xff00) == 0x0300)
    return Cycles(2); // MULSU / FMUL / FMULS / FMULSU

  // ---- 16-bit immediate add/sub and I/O bit ops (2 cycles) ----
  if ((op & 0xfe00) == 0x9600)
    return Cycles(2); // ADIW / SBIW
  if ((op & 0xfd00) == 0x9800)
    return Cycles(2); // CBI / SBI

  // ---- conditional skips: 1 (no skip), 2 (skip 1 word), 3 (skip 2 words) ----
  bool isSkip = ((op & 0xfc00) == 0x1000) ||  // CPSE
                ((op & 0xfc08) == 0xfc00) ||  // SBRC / SBRS
                ((op & 0xfd00) == 0x9900);    // SBIC / SBIS
  if (isSkip) {
    Addr d = newPc - oldPc;
    if (d == 6)
      return Cycles(3);
    if (d == 4)
      return Cycles(2);
    return Cycles(1);
  }

  // ---- conditional branches (BRBC/BRBS): 1 not-taken, 2 taken ----
  if ((op & 0xf800) == 0xf000)
    return (newPc == oldPc + 2) ? Cycles(1) : Cycles(2);

  // ---- everything else (ALU, logic, mov, ldi, in, out, ...) ----
  return Cycles(1);
}

void AVRCPU::dumpOpMix() {
  if (opHist.empty())
    return;
  // Sort by descending count so the dominant instructions ("what they use
  // most") come first.
  std::vector<std::pair<std::string, uint64_t>> v(opHist.begin(), opHist.end());
  std::sort(v.begin(), v.end(),
            [](const std::pair<std::string, uint64_t> &a,
               const std::pair<std::string, uint64_t> &b) {
              return a.second > b.second;
            });
  uint64_t total = 0;
  for (const auto &kv : v)
    total += kv.second;

  OutputStream *os = simout.create("avr_opmix.txt", false);
  std::ostream &s = *os->stream();
  s << "# AVR per-mnemonic execution histogram (op-mix)\n";
  s << "# columns: mnemonic count\n";
  for (const auto &kv : v)
    s << kv.first << ' ' << kv.second << '\n';
  s << "# total " << total << '\n';
  simout.close(os);
}

} // namespace gem5
