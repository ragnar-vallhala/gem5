#include "arch/avr/faults.hh"
#include "cpu/thread_context.hh"
#include "arch/avr/pcstate.hh"  // Add this include


namespace gem5
{
namespace AVRISAInst
{

void
AVRFault::returnFromFault(ThreadContext *tc)
{
    // Basic implementation
    auto pc = tc->pcState().as<PCState>();
    pc.advance();
    tc->pcState(pc);
}

void
AVRFault::invoke(ThreadContext *tc, const StaticInstPtr &inst)
{
    panic("AVR fault occurred: %s\n", name());
}

void
UnknownInstFault::invoke(ThreadContext *tc, const StaticInstPtr &inst)
{
    panic("Unknown instruction fault occurred\n");
}

} // namespace AVRISAInst
} // namespace gem5
