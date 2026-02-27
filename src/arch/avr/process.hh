#ifndef __ARCH_AVR_PROCESS_HH__
#define __ARCH_AVR_PROCESS_HH__

#include "sim/process.hh"

namespace gem5 {

class AVRProcess : public Process {
public:
  AVRProcess(const ProcessParams &params, loader::ObjectFile *objFile);

  void initState() override;

private:
  void argsInit(int pageSize);
};

} // namespace gem5

#endif // __ARCH_AVR_PROCESS_HH__
