#ifndef __ARCH_AVR_UTILITY_HH__
#define __ARCH_AVR_UTILITY_HH__

#include "arch/avr/registers.hh"
#include "arch/avr/types.hh"
#include "base/types.hh"
#include <cstdint>

namespace gem5 {

namespace AVRISAInst {

// Utility functions for instruction execution
uint8_t bits(const MachInst machInst, int hi, int lo);
void updateFlagsAdd(uint8_t &sreg, uint8_t result, uint8_t rd, uint8_t rr);
void updateFlagsAddC(uint8_t &sreg, uint8_t result, uint8_t rd, uint8_t rr,
                     uint8_t carry);
void updateFlagsSub(uint8_t &sreg, uint8_t result, uint8_t rd, uint8_t rr);
void updateFlagsSubC(uint8_t &sreg, uint8_t result, uint8_t rd, uint8_t rr,
                     uint8_t carry);

} // namespace AVRISAInst
} // namespace gem5

#endif // __ARCH_AVR_UTILITY_HH__
