#include "arch/avr/registers.hh"
#include "debug/Decode.hh"  // Using existing Decode debug flag


namespace gem5
{
namespace AVRISAInst
{

const RegClass intRegClass(IntRegClass, "int", sizeof(uint32_t),
                         debug::Decode);  // Using the correct constructor

} // namespace AVRISAInst
} // namespace gem5
