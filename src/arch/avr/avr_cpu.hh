#ifndef ARCH_AVR_AVR_CPU_HH
#define ARCH_AVR_AVR_CPU_HH

#include "arch/avr/avr_interrupts.hh"
#include "arch/avr/avr_mmu.hh"
#include "arch/avr/types.hh"
#include "cpu/base.hh"
#include "cpu/exec_context.hh"
#include "cpu/simple_thread.hh"
#include "mem/port.hh"
#include "mem/request.hh"
#include "sim/eventq.hh"

#include <cstdint>
#include <string>
#include <unordered_map>

namespace gem5 {

class AVRCPUParams;

class AVRCPU : public BaseCPU {
public:
  AVRCPU(const AVRCPUParams &p);

  Port &getInstPort() override { return instPort; }
  Port &getDataPort() override { return dataPort; }

  void wakeup(ThreadID tid) override;
  void startup() override;

  void tick();
  // Executes one instruction and returns its cost in CPU cycles (AVR
  // instructions take 1-4 cycles depending on type; see instCycles()).
  Cycles executeInstruction();
  // Cycle cost of an AVR instruction per the instruction-set manual. machInst
  // is the (possibly 32-bit) encoding, is32 whether it is a 2-word op, and
  // oldPc/newPc bracket execution so taken branches and skips can be detected.
  Cycles instCycles(AVRISAInst::ExtMachInst machInst, bool is32, Addr oldPc,
                     Addr newPc) const;

  // Uniform extra cycles added to every data-memory access: a memory-latency
  // DSE knob (loosely motivated by, but not a faithful model of, XMEM wait
  // states -- it is not restricted to an external address range).
  // 0 = validated internal-SRAM timing.
  unsigned dataWaitStates;

  // Atomic memory access routed through the CPU ports so the SystemXBar
  // carries real fetch/load/store traffic. The access latency returned by
  // sendAtomic is intentionally ignored: instCycles() already models the
  // 2-cycle single-cycle-SRAM access, so adding port latency would
  // double-count. fetchAtomic uses instPort; data accesses use dataPort.
  void fetchAtomic(Addr paddr, uint8_t *data, unsigned size);
  void readDataAtomic(Addr paddr, uint8_t *data, unsigned size,
                      Request::Flags flags);
  void writeDataAtomic(Addr paddr, const uint8_t *data, unsigned size,
                       Request::Flags flags);

  Counter totalInsts() const override { return insts; }
  Counter totalOps() const override { return ops; }

protected:
  EventFunctionWrapper tickEvent;

  /**
   * Minimal ExecContext adapter that wraps SimpleThread for use by
   * StaticInst::execute(). Only the methods needed by the AVR ISA
   * templates are fully implemented; the rest panic.
   */
  class AVRExecContext : public ExecContext {
  public:
    explicit AVRExecContext(SimpleThread *t) : thread(t) {}

    RegVal getRegOperand(const StaticInst *si, int idx) override {
      return thread->getReg(si->srcRegIdx(idx));
    }
    void getRegOperand(const StaticInst *si, int idx, void *val) override {
      thread->getReg(si->srcRegIdx(idx), val);
    }
    void *getWritableRegOperand(const StaticInst *si, int idx) override {
      return thread->getWritableReg(si->destRegIdx(idx));
    }
    void setRegOperand(const StaticInst *si, int idx, RegVal val) override {
      thread->setReg(si->destRegIdx(idx), val);
    }
    void setRegOperand(const StaticInst *si, int idx,
                       const void *val) override {
      thread->setReg(si->destRegIdx(idx), val);
    }

    RegVal readMiscRegOperand(const StaticInst *si, int idx) override {
      return thread->readMiscReg(si->srcRegIdx(idx).index());
    }
    void setMiscRegOperand(const StaticInst *si, int idx, RegVal val) override {
      thread->setMiscReg(si->destRegIdx(idx).index(), val);
    }
    RegVal readMiscReg(int misc_reg) override {
      return thread->readMiscReg(misc_reg);
    }
    void setMiscReg(int misc_reg, RegVal val) override {
      thread->setMiscReg(misc_reg, val);
    }

    const PCStateBase &pcState() const override { return thread->pcState(); }
    void pcState(const PCStateBase &val) override { thread->pcState(val); }

    Fault readMem(Addr addr, uint8_t *data, unsigned int size,
                  Request::Flags flags,
                  const std::vector<bool> &byte_enable) override {
      Addr phys_addr = addr;
      // If it's not an instruction fetch (LPM uses INST_FETCH flag here)
      // and it's a low address, map it to RAM (0x800000 offset).
      if (!(flags & Request::INST_FETCH) && addr < 0x800000) {
        phys_addr += 0x800000;
      }
      static_cast<AVRCPU *>(thread->getCpuPtr())
          ->readDataAtomic(phys_addr, data, size, flags);
      return NoFault;
    }

    Fault writeMem(uint8_t *data, unsigned int size, Addr addr,
                   Request::Flags flags, uint64_t *res,
                   const std::vector<bool> &byte_enable) override {
      Addr phys_addr = addr;
      if (addr < 0x800000) {
        phys_addr += 0x800000;
      }
      static_cast<AVRCPU *>(thread->getCpuPtr())
          ->writeDataAtomic(phys_addr, data, size, flags);
      if (res)
        *res = 0;
      return NoFault;
    }

    gem5::Fault initiateMemMgmtCmd(gem5::Request::Flags flags) override {
      return NoFault;
    }
    void setStCondFailures(unsigned int sc_failures) override {}
    unsigned int readStCondFailures() const override { return 0; }

    ThreadContext *tcBase() const override { return thread; }

    // ARM-specific stubs
    bool readPredicate() const override { return true; }
    void setPredicate(bool val) override {}
    bool readMemAccPredicate() const override { return true; }
    void setMemAccPredicate(bool val) override {}
    uint64_t newHtmTransactionUid() const override { return 0; }
    uint64_t getHtmTransactionUid() const override { return 0; }
    bool inHtmTransactionalState() const override { return false; }
    uint64_t getHtmTransactionalDepth() const override { return 0; }

    // x86-specific stubs
    void demapPage(Addr vaddr, uint64_t asn) override {}
    void armMonitor(Addr address) override {}
    bool mwait(PacketPtr pkt) override { return false; }
    void mwaitAtomic(ThreadContext *tc) override {}
    AddressMonitor *getAddrMonitor() override { return nullptr; }

  private:
    SimpleThread *thread;
  };

private:
  /* ---------------- Instruction Port (RequestPort) ---------------- */
  class AVRInstPort : public RequestPort {
  public:
    AVRInstPort(const std::string &name, AVRCPU *cpu)
        : RequestPort(name), cpu(cpu) {}

  protected:
    bool recvTimingResp(PacketPtr pkt) override { return true; }
    void recvReqRetry() override {}

  private:
    AVRCPU *cpu;
  };

  /* ---------------- Data Port (RequestPort) ---------------- */
  class AVRDataPort : public RequestPort {
  public:
    AVRDataPort(const std::string &name, AVRCPU *cpu)
        : RequestPort(name), cpu(cpu) {}

  protected:
    bool recvTimingResp(PacketPtr pkt) override { return true; }
    void recvReqRetry() override {}

  private:
    AVRCPU *cpu;
  };

  AVRInstPort instPort;
  AVRDataPort dataPort;

  AVRMMU *ArchMMU;
  AVRInterrupts *ArchInterrupts;

  Counter insts = 0;
  Counter ops = 0;

  // Workload-characterization histograms, written at simulation exit:
  //   opHist    -> avr_opmix.txt         (per-mnemonic instruction count = op-mix)
  //   opCycHist -> avr_opmix_cycles.txt  (per-mnemonic CYCLE count = where cycles go)
  //   funcHist  -> avr_funcmix.txt       (per-function instruction count, by PC symbol)
  std::unordered_map<std::string, uint64_t> opHist;
  std::unordered_map<std::string, uint64_t> opCycHist;
  std::unordered_map<std::string, uint64_t> funcHist;
  std::unordered_map<Addr, std::string> pcSymCache; // PC -> symbol name (memoized)
  std::string symbolFor(Addr pc);
  void dumpOpMix();
};

} // namespace gem5

#endif
