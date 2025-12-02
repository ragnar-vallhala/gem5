#include "arch/avr/avr_interrupts.hh"

namespace gem5
{

    // Constructor: call base class constructor
    AVRInterrupts::AVRInterrupts(const AVRInterruptsParams &p)
        : BaseInterrupts(p)
    {
    }

    // Pure virtual implementations
    bool
    AVRInterrupts::checkInterrupts() const
    {
        // Stub: no interrupts pending
        return false;
    }

    Fault
    AVRInterrupts::getInterrupt()
    {
        // Stub: return NoFault
        return NoFault;
    }

    void
    AVRInterrupts::updateIntrInfo()
    {
        // Stub: do nothing
    }

    // Old functions: override with empty stubs
    void
    AVRInterrupts::post(int int_num, int index)
    {
        // Do nothing for now
    }

    void
    AVRInterrupts::clear(int int_num, int index)
    {
        // Do nothing for now
    }

    void
    AVRInterrupts::clearAll()
    {
        // Do nothing for now
    }

} // namespace gem5
