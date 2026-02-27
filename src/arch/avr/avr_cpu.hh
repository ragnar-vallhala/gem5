#ifndef ARCH_AVR_AVR_CPU_HH
#define ARCH_AVR_AVR_CPU_HH

#include "arch/avr/avr_interrupts.hh"
#include "arch/avr/avr_mmu.hh"
#include "cpu/base.hh"
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
