#include "arch/avr/isa.hh"
#include "arch/avr/pcstate.hh"
#include "arch/avr/registers.hh"
#include "params/AVRISA.hh"

namespace gem5 {

AVR::AVR(const AVRISAParams &p) : BaseISA(p, "avr") {
  _regClasses.push_back(&AVRISAInst::intRegClass);
  _regClasses.push_back(&AVRISAInst::floatRegClass);
  _regClasses.push_back(&AVRISAInst::vecRegClass);
  _regClasses.push_back(&AVRISAInst::vecElemClass);
  _regClasses.push_back(&AVRISAInst::vecPredRegClass);
  _regClasses.push_back(&AVRISAInst::matRegClass);
  _regClasses.push_back(&AVRISAInst::ccRegClass);
  _regClasses.push_back(&AVRISAInst::miscRegClass);
  clear();
}

void AVR::clear() {
  // Initialize ISA state here if needed
}

PCStateBase *AVR::newPCState(Addr new_inst_addr) const {
  return new AVRISAInst::PCState(new_inst_addr);
}

RegVal AVR::readMiscRegNoEffect(RegIndex idx) const {
  return 0; // Stub
}

RegVal AVR::readMiscReg(RegIndex idx) {
  return 0; // Stub
}

void AVR::setMiscRegNoEffect(RegIndex idx, RegVal val) {
  // Stub
}

void AVR::setMiscReg(RegIndex idx, RegVal val) {
  // Stub
}

bool AVR::inUserMode() const {
  return false; // Stub
}

void AVR::copyRegsFrom(ThreadContext *src) {
  // Stub
}

} // namespace gem5
