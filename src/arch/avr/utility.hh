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

/* Read a 4-byte little-endian register quad as a raw word. */
#define FP_RAW(R, OUT)                                                        \
    do {                                                                      \
        (OUT) = 0;                                                            \
        for (int _i = 0; _i < 4; ++_i)                                        \
            (OUT) |= ((uint32_t)(uint8_t)xc->tcBase()->getReg(                \
                RegId(gem5::AVRISAInst::intRegClass, (R) + _i))) << (8 * _i); \
    } while (0)

/* Write a raw word back across a 4-byte register quad. */
#define FP_PUT(R, VAL)                                                        \
    do {                                                                      \
        for (int _i = 0; _i < 4; ++_i)                                        \
            xc->tcBase()->setReg(                                             \
                RegId(gem5::AVRISAInst::intRegClass, (R) + _i),               \
                (RegVal)(uint8_t)((VAL) >> (8 * _i)));                        \
    } while (0)

/*
 * Unary float op: Rd = EXPR(a). Used for the conversions and negate.
 * CAST names the type the result word should be reinterpreted from, so one
 * macro covers float->int, int->float and float->float.
 */
#define FP_UNOP(RD, DECL, EXPR)                                               \
    do {                                                                      \
        const uint8_t _rd = (uint8_t)(RD);                                    \
        uint32_t _ua;                                                         \
        FP_RAW(_rd, _ua);                                                     \
        float a;                                                              \
        std::memcpy(&a, &_ua, 4);                                             \
        (void)a;                                                              \
        DECL;                                                                 \
        uint32_t _ur;                                                         \
        std::memcpy(&_ur, &(EXPR), 4);                                        \
        FP_PUT(_rd, _ur);                                                     \
    } while (0)

/*
 * Three-way float compare into an int16 at Rt..Rt+1.
 *
 * libgcc's soft-float comparison helpers (__ltsf2, __lesf2, __gtsf2, __gesf2,
 * __eqsf2, __nesf2, __cmpsf2) all share one underlying result: negative if
 * a<b, zero if equal, positive if a>b, and -- crucially -- a POSITIVE value
 * when either operand is NaN, which is what makes every ordered predicate come
 * out false on unordered input. Returning 1 for unordered therefore lets a
 * single instruction serve all seven entry points.
 */
#define FP_CMP(RD, RR, RT)                                                    \
    do {                                                                      \
        const uint8_t _rd = (uint8_t)(RD), _rr = (uint8_t)(RR),               \
                      _rt = (uint8_t)(RT);                                    \
        uint32_t _ua, _ub;                                                    \
        FP_RAW(_rd, _ua);                                                     \
        FP_RAW(_rr, _ub);                                                     \
        float a, b;                                                           \
        std::memcpy(&a, &_ua, 4);                                             \
        std::memcpy(&b, &_ub, 4);                                             \
        int16_t _res;                                                         \
        if (a < b)       _res = -1;                                           \
        else if (a == b) _res = 0;                                            \
        else             _res = 1; /* covers a>b and unordered */             \
        xc->tcBase()->setReg(RegId(gem5::AVRISAInst::intRegClass, _rt),       \
                             (RegVal)(uint8_t)(_res & 0xff));                 \
        xc->tcBase()->setReg(RegId(gem5::AVRISAInst::intRegClass, _rt + 1),   \
                             (RegVal)(uint8_t)((_res >> 8) & 0xff));          \
    } while (0)

#endif // __ARCH_AVR_UTILITY_HH__
