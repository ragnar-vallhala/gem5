#ifndef __ARCH_AVR_INSTS_STATICINST_HH__
#define __ARCH_AVR_INSTS_STATICINST_HH__

#include <cstddef>
#include <sstream>
#include <string>

#include "arch/avr/registers.hh"
#include "arch/avr/types.hh"
#include "arch/generic/pcstate.hh"
#include "base/logging.hh"
#include "base/refcnt.hh"
#include "cpu/op_class.hh"
#include "cpu/reg_class.hh"
#include "cpu/static_inst.hh"
#include "cpu/static_inst_fwd.hh"
#include "enums/StaticInstFlags.hh"
#include "sim/byteswap.hh"

namespace gem5 {
namespace AVRISAInst {

class AVRStaticInst : public StaticInst {
protected:
  gem5::AVRISAInst::MachInst machInst;

  // Register indices arrays required by StaticInst
  RegId _srcRegIdx[4];
  RegId _destRegIdx[2];

public:
  AVRStaticInst(const char *mnem, gem5::AVRISAInst::MachInst _machInst,
                gem5::enums::OpClass __opClass)
      : StaticInst(mnem, __opClass), machInst(_machInst) {
    setRegIdxArrays(
        reinterpret_cast<RegIdArrayPtr>(&AVRStaticInst::_srcRegIdx),
        reinterpret_cast<RegIdArrayPtr>(&AVRStaticInst::_destRegIdx));
  }

  virtual ~AVRStaticInst() {}

  virtual size_t asBytes(void *buf, size_t max_size) override { return _size; }

  virtual void advancePC(PCStateBase &pc_state) const override = 0;

  virtual std::string generateDisassembly(
      gem5::Addr pc,
      const gem5::loader::SymbolTable *symtab) const override = 0;

  // Helpers for reading register indices as raw values if needed
  size_t srcRegIdx(int idx) const { return StaticInst::srcRegIdx(idx).index(); }
  size_t destRegIdx(int idx) const {
    return StaticInst::destRegIdx(idx).index();
  }
};

} // namespace AVRISAInst
} // namespace gem5
#endif // __ARCH_AVR_INSTS_STATICINST_HH__
