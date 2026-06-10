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
  for (int i = 0; i < AVRISAInst::NUM_MISCREGS; i++) {
    miscRegs[i] = 0;
  }
}

PCStateBase *AVR::newPCState(Addr new_inst_addr) const {
  return new AVRISAInst::PCState(new_inst_addr);
}

RegVal AVR::readMiscRegNoEffect(RegIndex idx) const {
  if (idx >= AVRISAInst::NUM_MISCREGS)
    return 0;
  return miscRegs[idx];
}

RegVal AVR::readMiscReg(RegIndex idx) {
  if (idx >= AVRISAInst::NUM_MISCREGS)
    return 0;
  return miscRegs[idx];
}

void AVR::setMiscRegNoEffect(RegIndex idx, RegVal val) {
  if (idx < AVRISAInst::NUM_MISCREGS)
    miscRegs[idx] = val;
}

void AVR::setMiscReg(RegIndex idx, RegVal val) {
  if (idx < AVRISAInst::NUM_MISCREGS)
    miscRegs[idx] = val;
}

bool AVR::inUserMode() const {
  return false; // Stub
}

void AVR::copyRegsFrom(ThreadContext *src) {
  // Stub
}

} // namespace gem5
