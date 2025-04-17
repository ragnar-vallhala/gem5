#ifndef __ARCH_AVR_PCSTATE_HH__
#define __ARCH_AVR_PCSTATE_HH__

#include "arch/generic/pcstate.hh"

namespace gem5
{
    namespace AVRISAInst
    {
        class PCState : public PCStateBase
        {
        public:
            PCState() : PCStateBase() {}
            PCState(Addr val) { set(val); }

            PCStateBase *clone() const override { return new PCState(*this); }
            void advance() override { _pc += 2; } // AVR uses 2-byte instructions
            bool branching() const override { return false; }
            void output(std::ostream &os) const override { os << "PC:" << _pc; }
        };
    } // namespcace AVRISAInst

} // namespace gem5

#endif // __ARCH_AVR_PCSTATE_HH__
