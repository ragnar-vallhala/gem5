#ifndef __ARCH_AVR_AVR_MMU_HH__
#define __ARCH_AVR_AVR_MMU_HH__

#include "arch/generic/mmu.hh"
#include "params/AVRMMU.hh"

namespace gem5 {

/**
 * Minimal AVR MMU: identity mapping (virtual == physical).
 *
 * This class implements the minimal set of pure virtual functions from
 * BaseMMU so it is non-abstract and can be used as a simple passthrough MMU
 * for AVR simulations that don't need real virtual memory.
 */
class AVRMMU : public BaseMMU
{
  public:
    using Params = AVRMMUParams;

    explicit AVRMMU(const Params &p);

    // Functional translation: return a TranslationGenPtr for the range.
    TranslationGenPtr translateFunctional(
        Addr start, Addr size,
        ThreadContext *tc,
        BaseMMU::Mode mode,
        Request::Flags flags) override;

    // Atomic translation (synchronous)
    Fault translateAtomic(const RequestPtr &req,
                          ThreadContext *tc,
                          Mode mode) override;

    // Timing translation (asynchronous path)
    void translateTiming(const RequestPtr &req,
                         ThreadContext *tc,
                         Translation *translation,
                         Mode mode) override;

    // Finalize physical (optional override); reuse base behaviour
    Fault finalizePhysical(const RequestPtr &req,
                           ThreadContext *tc,
                           Mode mode) const override
    {
        // Identity mapping: nothing special to finalize
        return NoFault;
    }

    // Other helpers are inherited from BaseMMU
};

} // namespace gem5

#endif // __ARCH_AVR_AVR_MMU_HH__
