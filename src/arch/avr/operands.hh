#ifndef __ARCH_AVR_OPERANDS_HH__
#define __ARCH_AVR_OPERANDS_HH__

#include "arch/avr/types.hh"
#include "base/types.hh"

namespace gem5
{

namespace AVRISAInst {

class AVROperand
{
  protected:
    uint32_t _value;

  public:
    AVROperand(uint32_t val) : _value(val) {}
    virtual ~AVROperand() {}

    uint32_t value() const { return _value; }

    virtual std::string generateDisassembly(Addr pc,
        const SymbolTable *symtab) const = 0;
};

class RegOperand : public AVROperand
{
  public:
    RegOperand(uint8_t reg) : AVROperand(reg) {}

    std::string
    generateDisassembly(Addr pc, const SymbolTable *symtab) const override
    {
        return csprintf("r%d", _value);
    }
};

class ImmOperand : public AVROperand
{
  public:
    ImmOperand(uint8_t imm) : AVROperand(imm) {}

    std::string
    generateDisassembly(Addr pc, const SymbolTable *symtab) const override
    {
        return csprintf("#%#x", _value);
    }
};

} // namespace AVRISAInst
} // namespace gem5

#endif // __ARCH_AVR_OPERANDS_HH__
