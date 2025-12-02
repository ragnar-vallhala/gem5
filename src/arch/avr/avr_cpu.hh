#ifndef ARCH_AVR_AVR_CPU_HH
#define ARCH_AVR_AVR_CPU_HH

#include "cpu/base.hh"
#include "mem/port.hh"

#include "sim/sim_object.hh"
#include "arch/avr/avr_mmu.hh"
#include "arch/avr/avr_interrupts.hh"

namespace gem5
{

  class AVRCPUParams;

  class AVRCPU : public BaseCPU
  {
  public:
    AVRCPU(const AVRCPUParams &p);
    ~AVRCPU() override = default;

    Port &getDataPort() override { return dport; }
    Port &getInstPort() override { return iport; }

    void wakeup(ThreadID tid) override;
    Counter totalInsts() const override { return insts; }
    Counter totalOps() const override { return ops; }

    void startup() override { BaseCPU::startup(); }
    AVRMMU *ArchMMU;
    AVRInterrupts *ArchInterrupts;

  private:
    // Concrete instruction port
    class AVRInstPort : public RequestPort
    {
    public:
      AVRInstPort(const std::string &name) : RequestPort(name) {}

    protected:
      bool recvTimingResp(PacketPtr pkt) override
      {
        panic("AVRCPU instruction port does not expect recvTimingResp!\n");
      }
      void recvReqRetry() override
      {
        panic("AVRCPU instruction port does not expect recvReqRetry!\n");
      }
    };

    // Concrete data port
    class AVRDataPort : public RequestPort
    {
    public:
      AVRDataPort(const std::string &name, BaseCPU *cpu) : RequestPort(name, cpu) {}

    protected:
      bool recvTimingResp(PacketPtr pkt) override
      {
        panic("AVRCPU data port does not expect recvTimingResp!\n");
      }
      void recvReqRetry() override
      {
        panic("AVRCPU data port does not expect recvReqRetry!\n");
      }
    };

    AVRInstPort iport;
    AVRDataPort dport;

    Counter insts = 0;
    Counter ops = 0;
    void executeInstruction();
  };

} // namespace gem5

#endif // ARCH_AVR_AVR_CPU_HH
