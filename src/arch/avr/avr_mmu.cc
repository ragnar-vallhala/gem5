#include "arch/avr/avr_mmu.hh"
#include "params/AVRMMU.hh"

namespace gem5 {

/** Constructor */
AVRMMU::AVRMMU(const AVRMMUParams &p)
    : BaseMMU(p)
{
    // Nothing special required. BaseMMU ctor initializes ITB/DTB from params.
}

/**
 * Functional translation generator for a region [start, start+size).
 * We return a MMUTranslationGen (provided by BaseMMU) configured for an
 * identity mapping (virtual == physical).
 *
 * NOTE: The 'page_bytes' argument to MMUTranslationGen is the page size
 * used by the generator for iterating ranges. We choose '1' (byte granularity)
 * for a simple identity behavior on small microcontroller address spaces.
 */
TranslationGenPtr
AVRMMU::translateFunctional(Addr start, Addr size,
                            ThreadContext *tc,
                            BaseMMU::Mode mode,
                            Request::Flags flags)
{
    // Use small page_bytes (1) to ensure the translation generator covers
    // the requested range without coalescing or assumptions about page size.
    const Addr page_bytes = 1;

    return TranslationGenPtr(
        new MMUTranslationGen(page_bytes, start, size, tc, this, mode, flags));
}

/**
 * Atomic translation: resolve the request immediately and set the physical
 * address to the virtual address (identity mapping).
 */
Fault
AVRMMU::translateAtomic(const RequestPtr &req, ThreadContext *tc, Mode mode)
{
    // Identity mapping: physical == virtual.
    // Request provides getVaddr() / setPaddr()
    req->setPaddr(req->getVaddr());

    // Optionally call finalizePhysical if needed. BaseMMU::finalizePhysical
    // will handle any final adjustments; here it returns NoFault by default.
    return NoFault;
}

/**
 * Timing translation: perform identity mapping and finish the Translation
 * object (synchronous here).
 */
void
AVRMMU::translateTiming(const RequestPtr &req, ThreadContext *tc,
                        Translation *translation, Mode mode)
{
    // Identity mapping
    req->setPaddr(req->getVaddr());

    // Finish the translation immediately with NoFault.
    translation->finish(NoFault, req, tc, mode);
}

} // namespace gem5
