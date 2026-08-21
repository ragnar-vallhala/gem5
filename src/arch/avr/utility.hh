#ifndef __ARCH_AVR_UTILITY_HH__
#define __ARCH_AVR_UTILITY_HH__

#include <cstring>

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

/*
 * AVR-X floating-point extension helper.
 *
 * Rd and Rr name 4-byte little-endian groups in the general register file
 * rather than a separate vector file. That is deliberate: the avr-gcc float
 * ABI already delivers operands as register quads (arg0/result in R22-R25,
 * arg1 in R18-R21), so a softfloat entry point can be replaced by a stub of
 * one FP instruction plus a ret -- no compiler change, no workload change, and
 * no register-file design decision blocking the timing experiment.
 *
 * The bitfield selectors are passed in rather than referenced here: the ISA
 * parser substitutes bitfield names into the instruction's code text, so a
 * macro body in this header would never see them.
 *
 * Semantics are the host's IEEE-754 single precision, which is the same
 * standard avr-gcc's softfloat implements -- so results must be bit-identical
 * to the library being replaced, and that is checkable (see the AVR-X
 * validation harness) even though the timing cannot be checked against
 * silicon, the ATmega328P having no FPU.
 */
#define FP_BINOP(RD, RR, EXPR)                                                \
    do {                                                                      \
        const uint8_t _rd = (uint8_t)(RD), _rr = (uint8_t)(RR);               \
        uint32_t _ua = 0, _ub = 0;                                            \
        for (int _i = 0; _i < 4; ++_i) {                                      \
            _ua |= ((uint32_t)(uint8_t)xc->tcBase()->getReg(                  \
                RegId(gem5::AVRISAInst::intRegClass, _rd + _i))) << (8 * _i); \
            _ub |= ((uint32_t)(uint8_t)xc->tcBase()->getReg(                  \
                RegId(gem5::AVRISAInst::intRegClass, _rr + _i))) << (8 * _i); \
        }                                                                     \
        float a, b;                                                           \
        std::memcpy(&a, &_ua, 4);                                             \
        std::memcpy(&b, &_ub, 4);                                             \
        const float _res = (EXPR);                                            \
        uint32_t _ur;                                                         \
        std::memcpy(&_ur, &_res, 4);                                          \
        for (int _i = 0; _i < 4; ++_i) {                                      \
            xc->tcBase()->setReg(                                             \
                RegId(gem5::AVRISAInst::intRegClass, _rd + _i),               \
                (RegVal)(uint8_t)(_ur >> (8 * _i)));                          \
        }                                                                     \
    } while (0)

#endif // __ARCH_AVR_UTILITY_HH__
