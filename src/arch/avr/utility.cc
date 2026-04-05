#include "arch/avr/utility.hh"

namespace gem5 {

namespace AVRISAInst {

uint8_t bits(const MachInst machInst, int hi, int lo) {
  return (machInst >> lo) & ((1 << (hi - lo + 1)) - 1);
}

void updateFlagsAdd(uint8_t &sreg, uint8_t result, uint8_t rd, uint8_t rr) {
  bool n = (result & 0x80);
  bool v = ((rd & 0x80) && (rr & 0x80) && !(result & 0x80)) ||
           (!(rd & 0x80) && !(rr & 0x80) && (result & 0x80));
  bool z = (result == 0);
  bool c = ((uint16_t)rd + (uint16_t)rr) > 0xFF;
  bool s = n ^ v;

  sreg = (sreg & 0xE0) | (s << SREG_S) | (n << SREG_N) | (v << SREG_V) |
         (z << SREG_Z) | (c << SREG_C);
}

void updateFlagsSub(uint8_t &sreg, uint8_t result, uint8_t rd, uint8_t rr) {
  bool n = (result & 0x80);
  bool v = ((rd & 0x80) && !(rr & 0x80) && !(result & 0x80)) ||
           (!(rd & 0x80) && (rr & 0x80) && (result & 0x80));
  bool z = (result == 0);
  bool c = (uint8_t)rr > (uint8_t)rd;
  bool s = n ^ v;

  sreg = (sreg & 0xE0) | (s << SREG_S) | (n << SREG_N) | (v << SREG_V) |
         (z << SREG_Z) | (c << SREG_C);
}

void updateFlagsSubC(uint8_t &sreg, uint8_t result, uint8_t rd, uint8_t rr,
                     uint8_t carry) {
  bool n = (result & 0x80);
  bool v = ((rd & 0x80) && !(rr & 0x80) && !(result & 0x80)) ||
           (!(rd & 0x80) && (rr & 0x80) && (result & 0x80));
  bool z = (result == 0) && (sreg & (1 << SREG_Z));
  bool c = (uint16_t)rr + carry > (uint16_t)rd;
  bool s = n ^ v;

  sreg = (sreg & 0xE0) | (s << SREG_S) | (v << SREG_V) | (n << SREG_N) |
         (z << SREG_Z) | (c << SREG_C);
}

} // namespace AVRISAInst
} // namespace gem5
