#ifndef __ARCH_AVR_SE_WORKLOAD_HH__
#define __ARCH_AVR_SE_WORKLOAD_HH__

#include "base/loader/object_file.hh"
#include "params/AVRSEWorkload.hh"
#include "sim/se_workload.hh"

namespace gem5 {

namespace AVR {

class SEWorkload : public gem5::SEWorkload {
public:
  using Params = AVRSEWorkloadParams;

  SEWorkload(const Params &p) : gem5::SEWorkload(p, 12) {} // 12 = log2(4096)

  ByteOrder byteOrder() const override { return ByteOrder::little; }

  loader::Arch getArch() const override { return loader::Avr; }

  // AVR bare-metal does not use syscalls
  void syscall(ThreadContext *tc) override {
    warn("AVRSEWorkload: syscall not supported on AVR.\n");
  }
};

} // namespace AVR
} // namespace gem5

#endif // __ARCH_AVR_SE_WORKLOAD_HH__
