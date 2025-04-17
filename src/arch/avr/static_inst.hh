#ifndef __ARCH_AVR_INSTS_STATICINST_HH__
#define __ARCH_AVR_INSTS_STATICINST_HH__

#include "arch/avr/registers.hh"
#include "cpu/static_inst.hh"
#include "arch/avr/types.hh"
#include "arch/generic/pcstate.hh"
#include "base/logging.hh"
#include "base/refcnt.hh"
#include "cpu/op_class.hh"
#include "cpu/reg_class.hh"
#include "cpu/static_inst_fwd.hh"
#include "enums/StaticInstFlags.hh"
#include "sim/byteswap.hh"

namespace gem5
{
  namespace AVRISAInst
  {
    class AVRStaticInst : public StaticInst
    {
    protected:
      gem5::AVRISAInst::MachInst machInst;

      // Register indices
      int8_t _srcRegIdx[2];
      int8_t _destRegIdx[1];
      int8_t _numSrcRegs;
      int8_t _numDestRegs;

    public:
      AVRStaticInst(const char *mnem, gem5::AVRISAInst::MachInst _machInst, gem5::enums::OpClass __opClass)
          : StaticInst(mnem, __opClass), machInst(_machInst)
      {
        _numSrcRegs = 0;
        _numDestRegs = 0;
      }

      ~AVRStaticInst() {}

      virtual size_t asBytes(void *buf, size_t size) override { return 2; } // AVR instructions are 2 bytes
      void advancePC(PCStateBase &pc_state) const = 0;
      std::string generateDisassembly(
          gem5::Addr pc,
          const gem5::loader::SymbolTable *symtab) const override = 0;

      size_t srcRegIdx(int idx) const { return _srcRegIdx[idx]; }
      size_t destRegIdx(int idx) const { return _destRegIdx[idx]; }
      size_t numSrcRegs() const { return _numSrcRegs; }
      size_t numDestRegs() const { return _numDestRegs; }
    };
  }
}
#endif // __ARCH_AVR_INSTS_STATICINST_HH__
