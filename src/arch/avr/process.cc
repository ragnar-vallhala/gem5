#include "arch/avr/process.hh"

#include "arch/avr/registers.hh"
#include "base/loader/object_file.hh"
#include "base/logging.hh"
#include "cpu/thread_context.hh"
#include "mem/page_table.hh"
#include "params/Process.hh"
#include "sim/process.hh"
#include "sim/system.hh"

namespace gem5 {

// AVR uses 64KB address space (16-bit), page size can be 256 bytes.
// For simulation purposes we use a 4KB page size.
static constexpr Addr AvrPageBytes = 4096;

AVRProcess::AVRProcess(const ProcessParams &params, loader::ObjectFile *objFile)
    : Process(params,
              new EmulationPageTable(params.name, params.pid, AvrPageBytes),
              objFile) {
  fatal_if(params.useArchPT, "Arch page tables not implemented for AVR.");

  // AVR uses a 16-bit address space (64KB).
  // Use 0x80FFFF as max stack address.
  const Addr stack_base = 0x80FFFF;
  const Addr max_stack_size = 4 * 1024; // 4KB max stack
  const Addr next_thread_stack_base = stack_base - max_stack_size;
  const Addr brk_point = roundUp(image.maxAddr(), AvrPageBytes);
  const Addr mmap_end = 0x808000;
  memState =
      std::make_shared<MemState>(this, brk_point, stack_base, max_stack_size,
                                 next_thread_stack_base, mmap_end);
}

void AVRProcess::initState() {
  Process::initState();
  argsInit(AvrPageBytes);
}

void AVRProcess::argsInit(int pageSize) {
  // Very simple stack setup: set SP to stack base and PC to entry point.
  ThreadContext *tc = system->threads[contextIds[0]];
  tc->pcState(getStartPC());
  tc->setMiscReg(AVRISAInst::MISCREG_SP, memState->getStackBase());
}

// -----------------------------------------------------------------------
// Process::Loader registration for AVR
// -----------------------------------------------------------------------
namespace {

struct AVRLoader : public Process::Loader {
  Process *load(const ProcessParams &params,
                loader::ObjectFile *objFile) override {
    if (objFile->getArch() != loader::Avr)
      return nullptr;

    return new AVRProcess(params, objFile);
  }
} avrLoader;

} // anonymous namespace

} // namespace gem5
