
#include "arch/avr/isa.hh"
#include "params/AVR.hh"

namespace gem5
{

AVR::AVR(const Params &p) : SimObject(p)
{
    clear();
}

void
AVR::clear()
{
    // Initialize ISA state here if needed
}

} // namespace gem5
