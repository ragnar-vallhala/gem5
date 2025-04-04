#ifndef __ARCH_AVR_ISA_HH__
#define __ARCH_AVR_ISA_HH__

#include "arch/avr/types.hh"
#include "arch/generic/isa.hh"
#include "cpu/reg_class.hh"

namespace gem5
{

    namespace AvrISA
    {

        class ISA : public BaseISA
        {
        protected:
            RegIndex regSize = 32; // AVR has 32 general purpose registers

        public:
            using Params = AvrISAParams;

            ISA(Params *p) : BaseISA(p) {}

            RegId flattenRegId(const RegId &regId) const override
            {
                return regId;
            }
        };

    } // namespace AvrISA
} // namespace gem5

#endif // __ARCH_AVR_ISA_HH__
