#ifndef __ARCH_AVR_ISA_HH__
#define __ARCH_AVR_ISA_HH__

#include "arch/avr/registers.hh"
#include "arch/avr/types.hh"
#include "arch/generic/isa.hh"
#include "params/AVRISA.hh"

namespace gem5 {

class AVR : public BaseISA {
public:
  typedef AVRISAInst::MachInst MachInst;
  typedef AVRISAInst::ExtMachInst ExtMachInst;

  AVR(const AVRISAParams &p);

  void clear() override;

  PCStateBase *newPCState(Addr new_inst_addr = 0) const override;

  RegVal readMiscRegNoEffect(RegIndex idx) const override;
  RegVal readMiscReg(RegIndex idx) override;
  void setMiscRegNoEffect(RegIndex idx, RegVal val) override;
  void setMiscReg(RegIndex idx, RegVal val) override;

  bool inUserMode() const override;
  void copyRegsFrom(ThreadContext *src) override;

protected:
  RegVal miscRegs[AVRISAInst::NUM_MISCREGS];
};

} // namespace gem5

#endif // __ARCH_AVR_ISA_HH__
