#ifndef __ARCH_AVR_TYPES_HH__
#define __ARCH_AVR_TYPES_HH__

#include "base/logging.hh"
#include "base/types.hh"
#include "cpu/thread_context.hh"
#include "sim/faults.hh"

namespace gem5 {

namespace AVRISAInst {

// Basic types
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;

// Machine instruction type
typedef uint16_t MachInst;
typedef uint32_t ExtMachInst;

// Address type
typedef uint16_t Addr;

// Register types
typedef uint8_t RegVal;  // General purpose register value
typedef uint16_t PCVal;  // Program counter value
typedef uint16_t SPVal;  // Stack pointer value
typedef uint8_t SREGVal; // Status register value

// Exception types
enum ExceptionType {
  INT_Reset = 0,
  INT_External0 = 1,
  INT_External1 = 2,
  INT_Timer0 = 3,
  INT_Timer1 = 4,
  INT_USART = 5,
  // Add more interrupt types as needed
};

extern const RegClass intRegClass;
// Get the integer register class
const RegClass &getIntRegClass();

// Memory access size
enum MemAccessSize { Byte = 1, Word = 2 };

// Condition codes
enum ConditionCode {
  CondEQ = 0,  // Equal
  CondNE = 1,  // Not Equal
  CondCS = 2,  // Carry Set
  CondCC = 3,  // Carry Clear
  CondMI = 4,  // Minus
  CondPL = 5,  // Plus
  CondVS = 6,  // Overflow Set
  CondVC = 7,  // Overflow Clear
  CondHI = 8,  // Higher
  CondLS = 9,  // Lower or Same
  CondGE = 10, // Greater or Equal
  CondLT = 11, // Less Than
  CondGT = 12, // Greater Than
  CondLE = 13, // Less or Equal
  CondAL = 14  // Always
};

// Instruction formats
struct InstFormat {
  uint16_t opcode : 6;
  uint16_t rd : 5;
  uint16_t rr : 5;
};

// Status Register (SREG) bits
struct StatusRegister {
  uint8_t C : 1; // Carry Flag
  uint8_t Z : 1; // Zero Flag
  uint8_t N : 1; // Negative Flag
  uint8_t V : 1; // Overflow Flag
  uint8_t S : 1; // Sign Flag
  uint8_t H : 1; // Half Carry Flag
  uint8_t T : 1; // Transfer Flag
  uint8_t I : 1; // Global Interrupt Enable
};

// Fault classes
class AVRFault : public FaultBase {
protected:
  const char *_name;
  const uint8_t _code;

public:
  AVRFault(const char *name, uint8_t code) : _name(name), _code(code) {}

  virtual void invoke(ThreadContext *tc, const StaticInstPtr &inst = nullptr);
  virtual bool returnFromFault(ThreadContext *tc);
};

class UnknownInstFault : public AVRFault {
public:
  UnknownInstFault() : AVRFault("Unknown instruction", 0) {}
  const char *name() const override { return "UnknownInstFault"; }
  void invoke(ThreadContext *tc, const StaticInstPtr &inst = nullptr) override;
};

class UnalignedMemAccessFault : public AVRFault {
public:
  UnalignedMemAccessFault() : AVRFault("Unaligned memory access", 1) {}

  void invoke(ThreadContext *tc, const StaticInstPtr &inst = nullptr) override {
    panic("Unaligned memory access");
  }
};

class IllegalInstFault : public AVRFault {
public:
  IllegalInstFault() : AVRFault("Illegal instruction", 2) {}

  void invoke(ThreadContext *tc, const StaticInstPtr &inst = nullptr) override {
    panic("Illegal instruction");
  }
};

class PrivilegeViolationFault : public AVRFault {
public:
  PrivilegeViolationFault() : AVRFault("Privilege violation", 3) {}

  void invoke(ThreadContext *tc, const StaticInstPtr &inst = nullptr) override {
    panic("Privilege violation");
  }
};

// Instruction operation classes
enum OpClass {
  No_OpClass,
  IntAluOp,    // Integer ALU operation
  IntMultOp,   // Integer multiply operation
  IntDivOp,    // Integer divide operation
  FloatAddOp,  // Floating point add operation
  FloatCmpOp,  // Floating point compare operation
  FloatCvtOp,  // Floating point convert operation
  FloatMultOp, // Floating point multiply operation
  FloatDivOp,  // Floating point divide operation
  FloatSqrtOp, // Floating point square root operation
  MemReadOp,   // Memory read operation
  MemWriteOp,  // Memory write operation
  IprAccessOp, // Internal processor register access
  OpClassCount
};

// Memory request flags
enum RequestFlags {
  NORMAL = 0,
  STRICT_ORDER = 1,
  INST_FETCH = 2,
  PREFETCH = 4,
  UNCACHEABLE = 8
};

// Function to convert condition code to string (for debugging)
static inline const char *condCodeToString(ConditionCode cond) {
  switch (cond) {
  case CondEQ:
    return "EQ";
  case CondNE:
    return "NE";
  case CondCS:
    return "CS";
  case CondCC:
    return "CC";
  case CondMI:
    return "MI";
  case CondPL:
    return "PL";
  case CondVS:
    return "VS";
  case CondVC:
    return "VC";
  case CondHI:
    return "HI";
  case CondLS:
    return "LS";
  case CondGE:
    return "GE";
  case CondLT:
    return "LT";
  case CondGT:
    return "GT";
  case CondLE:
    return "LE";
  case CondAL:
    return "AL";
  default:
    return "??";
  }
}

// Function to check if a memory access is aligned
static inline bool isAligned(Addr addr, unsigned size) {
  return (addr & (size - 1)) == 0;
}

} // namespace AVRISAInst
} // namespace gem5

#endif // __ARCH_AVR_TYPES_HH__
