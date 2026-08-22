#ifndef __ARCH_AVR_UTILITY_HH__
#define __ARCH_AVR_UTILITY_HH__

#include <cmath>
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

/* Read a 2-byte little-endian register pair. */
#define FP_RAW16(R, OUT)                                                      \
    do {                                                                      \
        (OUT) = (uint16_t)((uint8_t)xc->tcBase()->getReg(                     \
                    RegId(gem5::AVRISAInst::intRegClass, (R))) |              \
                ((uint16_t)(uint8_t)xc->tcBase()->getReg(                     \
                    RegId(gem5::AVRISAInst::intRegClass, (R) + 1)) << 8));    \
    } while (0)

/*
 * 16x16 -> 32 integer multiply. Replaces __mulhisi3 / __umulhisi3 /
 * __usmulhisi3, which together account for the bulk of the integer-helper
 * cycles in the inference kernels -- work an FP unit does not touch at all.
 * SA/SB say whether each operand is signed, so one macro covers all three
 * signedness combinations libgcc provides.
 */
#define INT_MUL(RD, RR, RT, SA, SB)                                           \
    do {                                                                      \
        const uint8_t _rd = (uint8_t)(RD);                                    \
        uint16_t _a16, _b16;                                                  \
        FP_RAW16((uint8_t)(RR), _a16);                                        \
        FP_RAW16((uint8_t)(RT), _b16);                                        \
        const int64_t _a = (SA) ? (int64_t)(int16_t)_a16 : (int64_t)_a16;     \
        const int64_t _b = (SB) ? (int64_t)(int16_t)_b16 : (int64_t)_b16;     \
        const uint32_t _r = (uint32_t)(int32_t)(_a * _b);                     \
        FP_PUT(_rd, _r);                                                      \
    } while (0)

/*
 * 16/16 divide, quotient AND remainder, matching libgcc's __udivmodhi4 /
 * __divmodhi4: quotient at Rd..Rd+1, remainder at Rd+2..Rd+3. Those helpers
 * use a non-standard register convention (dividend R24:R25, divisor R23:R22,
 * quotient back in R23:R22 and remainder in R25:R24), which the stub encodes;
 * getting it wrong is caught by the byte-compare against softfloat rather than
 * by reasoning.
 *
 * Division by zero is left as whatever the host does for the same expression,
 * which matches the C standard's position that it is undefined -- the library
 * being replaced makes no guarantee either.
 */
#define INT_DIVMOD(RD, RR, RT, SGN)                                           \
    do {                                                                      \
        const uint8_t _rd = (uint8_t)(RD);                                    \
        uint16_t _n16, _d16;                                                  \
        FP_RAW16((uint8_t)(RR), _n16);                                        \
        FP_RAW16((uint8_t)(RT), _d16);                                        \
        uint16_t _q, _m;                                                      \
        if (_d16 == 0) {                                                      \
            _q = 0xffff;                                                      \
            _m = _n16;                                                        \
        } else if (SGN) {                                                     \
            const int16_t _n = (int16_t)_n16, _d = (int16_t)_d16;             \
            _q = (uint16_t)(int16_t)(_n / _d);                                \
            _m = (uint16_t)(int16_t)(_n % _d);                                \
        } else {                                                              \
            _q = (uint16_t)(_n16 / _d16);                                     \
            _m = (uint16_t)(_n16 % _d16);                                     \
        }                                                                     \
        xc->tcBase()->setReg(RegId(gem5::AVRISAInst::intRegClass, _rd),       \
                             (RegVal)(uint8_t)(_q & 0xff));                   \
        xc->tcBase()->setReg(RegId(gem5::AVRISAInst::intRegClass, _rd + 1),   \
                             (RegVal)(uint8_t)(_q >> 8));                     \
        xc->tcBase()->setReg(RegId(gem5::AVRISAInst::intRegClass, _rd + 2),   \
                             (RegVal)(uint8_t)(_m & 0xff));                   \
        xc->tcBase()->setReg(RegId(gem5::AVRISAInst::intRegClass, _rd + 3),   \
                             (RegVal)(uint8_t)(_m >> 8));                     \
    } while (0)

/*
 * Fused multiply-add: Rd = Rd + Rr * Rt, single rounding.
 *
 * Unlike every other instruction here this one has no libgcc entry point that
 * unmodified C reaches -- avr-gcc has no FMA to contract a*b+c into, so it
 * fires only through an explicit fmaf() call or hand-written code. It is
 * implemented because the encoding already had room for a third operand, and
 * measured separately rather than counted toward the drop-in speedups.
 */
#define FP_FMAC(RD, RR, RT)                                                   \
    do {                                                                      \
        const uint8_t _rd = (uint8_t)(RD);                                    \
        uint32_t _uc, _ua, _ub;                                               \
        FP_RAW(_rd, _uc);                                                     \
        FP_RAW((uint8_t)(RR), _ua);                                           \
        FP_RAW((uint8_t)(RT), _ub);                                           \
        float c, a, b;                                                        \
        std::memcpy(&c, &_uc, 4);                                             \
        std::memcpy(&a, &_ua, 4);                                             \
        std::memcpy(&b, &_ub, 4);                                             \
        const float _res = std::fma(a, b, c);                                 \
        uint32_t _ur;                                                         \
        std::memcpy(&_ur, &_res, 4);                                          \
        FP_PUT(_rd, _ur);                                                     \
    } while (0)

/* 32x32 -> 32 multiply (truncating), for libgcc's __mulsi3. */
#define INT_MUL32(RD, RR, RT)                                                 \
    do {                                                                      \
        uint32_t _a, _b;                                                      \
        FP_RAW((uint8_t)(RR), _a);                                            \
        FP_RAW((uint8_t)(RT), _b);                                            \
        const uint32_t _r = _a * _b;                                          \
        FP_PUT((uint8_t)(RD), _r);                                            \
    } while (0)

/*
 * 32/32 divide returning quotient AND remainder, for __udivmodsi4 /
 * __divmodsi4. Quotient goes to Rd..Rd+3 and remainder to Rd+4..Rd+7 --
 * contiguous, which is what lets a single Rd=18 land the quotient in R18-R21
 * and the remainder in R22-R25 exactly where those helpers return them.
 *
 * This is the helper that actually dominates the integer-heavy kernels: the
 * 16-bit divide covers cnt and arcfour, but fasta and prime spend ~90% of
 * their cycles in the 32-bit one.
 */
#define INT_DIVMOD32(RD, RR, RT, SGN)                                         \
    do {                                                                      \
        const uint8_t _rd = (uint8_t)(RD);                                    \
        uint32_t _n, _d;                                                      \
        FP_RAW((uint8_t)(RR), _n);                                            \
        FP_RAW((uint8_t)(RT), _d);                                            \
        uint32_t _q, _m;                                                      \
        if (_d == 0) {                                                        \
            _q = 0xffffffffu;                                                 \
            _m = _n;                                                          \
        } else if (SGN) {                                                     \
            const int32_t _sn = (int32_t)_n, _sd = (int32_t)_d;               \
            _q = (uint32_t)(_sn / _sd);                                       \
            _m = (uint32_t)(_sn % _sd);                                       \
        } else {                                                              \
            _q = _n / _d;                                                     \
            _m = _n % _d;                                                     \
        }                                                                     \
        FP_PUT(_rd, _q);                                                      \
        FP_PUT((uint8_t)(_rd + 4), _m);                                       \
    } while (0)

#endif // __ARCH_AVR_UTILITY_HH__
