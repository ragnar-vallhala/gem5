#include "arch/avr/utility.hh"

namespace gem5
{

namespace AVRISAInst {

uint8_t
bits(const MachInst machInst, int hi, int lo)
{
    return (machInst >> lo) & ((1 << (hi - lo + 1)) - 1);
}

void
updateFlagsAdd(uint8_t &sreg, uint8_t result, uint8_t rd, uint8_t rr)
{
    bool n = (result & 0x80);
    bool v = ((rd & 0x80) && (rr & 0x80) && !(result & 0x80)) ||
             (!(rd & 0x80) && !(rr & 0x80) && (result & 0x80));
    bool z = (result == 0);
    bool c = ((uint16_t)rd + (uint16_t)rr) > 0xFF;

    sreg = (sreg & 0xF0) |
           (n << SREG_N) |
           (v << SREG_V) |
           (z << SREG_Z) |
           (c << SREG_C);
}

void
updateFlagsSub(uint8_t &sreg, uint8_t result, uint8_t rd, uint8_t rr)
{
    bool n = (result & 0x80);
    bool v = ((rd & 0x80) && !(rr & 0x80) && !(result & 0x80)) ||
             (!(rd & 0x80) && (rr & 0x80) && (result & 0x80));
    bool z = (result == 0);
    bool c = rr > rd;

    sreg = (sreg & 0xF0) |
           (n << SREG_N) |
           (v << SREG_V) |
           (z << SREG_Z) |
           (c << SREG_C);
}

} // namespace AVRISAInst
} // namespace gem5
