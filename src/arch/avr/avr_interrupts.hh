#ifndef __ARCH_AVR_INTERRUPTS_HH__
#define __ARCH_AVR_INTERRUPTS_HH__

#include "arch/generic/interrupts.hh"
#include "params/AVRInterrupts.hh"

namespace gem5 {

class AVRInterrupts : public BaseInterrupts
{
  public:
    AVRInterrupts(const AVRInterruptsParams &p);

    // Pure virtuals from BaseInterrupts
    bool checkInterrupts() const override;
    Fault getInterrupt() override;
    void updateIntrInfo() override;

    // Old methods: override with empty stub
    void post(int int_num, int index) override;
    void clear(int int_num, int index) override;
    void clearAll() override;
};

} // namespace gem5

#endif
