#ifndef __ARCH_AVR_DECODER_HH__
#define __ARCH_AVR_DECODER_HH__

#include "arch/avr/types.hh"
#include "arch/generic/decoder.hh"
#include "cpu/static_inst.hh"
#include "params/AVRDecoder.hh"

namespace gem5 {

namespace AVRISAInst {

class Decoder : public InstDecoder {
protected:
  ExtMachInst emi;

public:
  Decoder(const AVRDecoderParams &p) : InstDecoder(p, &emi) {}

  void process() {}
  void reset() override {}

  void moreBytes(const PCStateBase &pc, gem5::Addr fetchPC) override {}

  StaticInstPtr decodeInst(ExtMachInst mach_inst);

  StaticInstPtr decode(ExtMachInst mach_inst, Addr addr) {
    return decodeInst(mach_inst);
  }

  StaticInstPtr decode(PCStateBase &nextPC) override { return decodeInst(emi); }
};

} // namespace AVRISAInst
} // namespace gem5

#endif // __ARCH_AVR_DECODER_HH__
