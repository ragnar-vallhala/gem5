#ifndef ARCH_AVR_AVR_CPU_HH
#define ARCH_AVR_AVR_CPU_HH

#include "arch/avr/avr_interrupts.hh"
#include "arch/avr/avr_mmu.hh"
#include "cpu/base.hh"
#include "cpu/exec_context.hh"
#include "cpu/simple_thread.hh"
#include "mem/port.hh"
#include "sim/eventq.hh"

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
  void executeInstruction();
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
      for (unsigned int i = 0; i < size; i++) {
        data[i] = thread->getSystemPtr()->physProxy.read<uint8_t>(addr + i);
      }
      return NoFault;
    }

    Fault writeMem(uint8_t *data, unsigned int size, Addr addr,
                   Request::Flags flags, uint64_t *res,
                   const std::vector<bool> &byte_enable) override {
      for (unsigned int i = 0; i < size; i++) {
        thread->getSystemPtr()->physProxy.write<uint8_t>(addr + i, data[i]);
      }
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
};

} // namespace gem5

#endif
