#include "arch/avr/registers.hh"
#include "debug/Decode.hh" // Using existing Decode debug flag

namespace gem5 {
namespace AVRISAInst {

const RegClass intRegClass(IntRegClass, IntRegClassName, NumRegs,
                           debug::Decode);
const RegClass floatRegClass(FloatRegClass, FloatRegClassName, 0,
                             debug::Decode);
const RegClass vecRegClass(VecRegClass, VecRegClassName, 0, debug::Decode);
const RegClass vecElemClass(VecElemClass, VecElemClassName, 0, debug::Decode);
const RegClass vecPredRegClass(VecPredRegClass, VecPredRegClassName, 0,
                               debug::Decode);
const RegClass matRegClass(MatRegClass, MatRegClassName, 0, debug::Decode);
const RegClass ccRegClass(CCRegClass, CCRegClassName, 0, debug::Decode);
const RegClass miscRegClass(MiscRegClass, MiscRegClassName, NUM_MISCREGS,
                            debug::Decode);

} // namespace AVRISAInst
} // namespace gem5
