#ifndef ARCH_AVR_AVR_CPU_HH
#define ARCH_AVR_AVR_CPU_HH

#include "cpu/base.hh"
#include "mem/port.hh"
#include "mem/tport.hh"
#include "arch/avr/avr_mmu.hh"
#include "arch/avr/avr_interrupts.hh"

namespace gem5
{

  class AVRCPUParams;

  class AVRCPU : public BaseCPU
  {
  public:
    AVRCPU(const AVRCPUParams &p);

    Port &getInstPort() override { return instPort; }
    Port &getDataPort() override { return dataPort; }

    void wakeup(ThreadID tid) override {}

    void tick();
    void executeInstruction();
    Counter totalInsts() const override { return insts; }
    Counter totalOps() const override { return ops; }

  private:
    /* ---------------- Instruction Port ---------------- */
    class AVRInstPort : public SimpleTimingPort
    {
    public:
      AVRInstPort(const std::string &name, AVRCPU *cpu)
          : SimpleTimingPort(name, cpu), cpu(cpu) {}

    private:
      AVRCPU *cpu;

      AddrRangeList getAddrRanges() const override;
      Tick recvAtomic(PacketPtr pkt) override;
    };

    /* ---------------- Data Port ---------------- */
    class AVRDataPort : public SimpleTimingPort
    {
    public:
      AVRDataPort(const std::string &name, AVRCPU *cpu)
          : SimpleTimingPort(name, cpu), cpu(cpu) {}

    private:
      AVRCPU *cpu;
      AddrRangeList getAddrRanges() const override;
      Tick recvAtomic(PacketPtr pkt) override;
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
