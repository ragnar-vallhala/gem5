#ifndef __ARCH_AVR_FAULTS_HH__
#define __ARCH_AVR_FAULTS_HH__

#include "sim/faults.hh"

namespace gem5
{

class ThreadContext;

namespace AVRISAInst
{

class AVRFault : public FaultBase
{
  public:
    void returnFromFault(ThreadContext *tc);
    FaultName name() const override { return "AVRFault"; }
    void invoke(ThreadContext *tc, const StaticInstPtr &inst) override;
};

class UnknownInstFault : public AVRFault
{
  public:
    FaultName name() const override { return "UnknownInstFault"; }
    void invoke(ThreadContext *tc, const StaticInstPtr &inst) override;
};

} // namespace AVRISAInst
} // namespace gem5

#endif
