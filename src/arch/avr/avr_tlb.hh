#ifndef __ARCH_AVR_AVR_TLB_HH__
#define __ARCH_AVR_AVR_TLB_HH__

#include "arch/generic/mmu.hh"
#include "arch/generic/tlb.hh"
#include "params/AVRTLB.hh"

namespace gem5 {

class AVRTLB : public BaseTLB {
public:
  using Params = AVRTLBParams;

  explicit AVRTLB(const Params &p) : BaseTLB(p) {}

  Fault translateFunctional(const RequestPtr &req, ThreadContext *tc,
                            BaseMMU::Mode mode) override {
    req->setPaddr(req->getVaddr());
    return NoFault;
  }

  Fault translateAtomic(const RequestPtr &req, ThreadContext *tc,
                        BaseMMU::Mode mode) override {
    req->setPaddr(req->getVaddr());
    return NoFault;
  }

  void translateTiming(const RequestPtr &req, ThreadContext *tc,
                       BaseMMU::Translation *translation,
                       BaseMMU::Mode mode) override {
    req->setPaddr(req->getVaddr());
    translation->finish(NoFault, req, tc, mode);
  }

  Fault finalizePhysical(const RequestPtr &req, ThreadContext *tc,
                         BaseMMU::Mode mode) const override {
    return NoFault;
  }

  void demapPage(Addr vaddr, uint64_t asn) override {}
  void flushAll() override {}
  void takeOverFrom(BaseTLB *otlb) override {}
};

} // namespace gem5

#endif // __ARCH_AVR_AVR_TLB_HH__
