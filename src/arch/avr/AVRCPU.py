# src/arch/avr/AVRCPU.py

from m5.objects import MasterPort
from m5.objects.AVRDecoder import AVRDecoder
from m5.objects.AVRInterrupts import AVRInterrupts
from m5.objects.AVRISA import AVRISA
from m5.objects.AVRMMU import AVRMMU
from m5.objects.BaseCPU import BaseCPU
from m5.params import Param


class AVRCPU(BaseCPU):
    """
    Python SimObject wrapper for the AVR CPU.
    Points to the C++ implementation `gem5::AVRCPU` in arch/avr/avr_cpu.hh.
    """

    type = "AVRCPU"
    cxx_class = "gem5::AVRCPU"
    cxx_header = "arch/avr/avr_cpu.hh"

    instPort = MasterPort("Instruction port to connect to memory")
    dataPort = MasterPort("Data port to connect to memory")

    # Uniform extra cycles charged on every data-memory access: a memory-latency
    # design-space-exploration knob. Loosely motivated by the AVR external-memory
    # interface (XMEM) wait states, but applied uniformly to all data accesses
    # rather than only to an external address range, so it is an abstraction, not
    # a faithful XMEM model. 0 keeps the validated internal-SRAM timing.
    dataWaitStates = Param.Unsigned(
        0, "Uniform extra cycles per data-memory access"
    )

    # AVR-X floating-point extension latencies. These are MODEL INPUTS, not
    # measurements: the ATmega328P has no FPU, so unlike every other cycle cost
    # in this model they cannot be validated against silicon. They are exposed
    # as parameters so the design study sweeps them and reports sensitivity
    # rather than quoting a single speedup that rests on an assumed latency.
    # Defaults are typical single-precision datapath figures.
    fpAddCycles = Param.Unsigned(4, "Cycles for AVR-X FADD.S / FSUB.S")
    fpMulCycles = Param.Unsigned(5, "Cycles for AVR-X FMUL.S")
    fpDivCycles = Param.Unsigned(16, "Cycles for AVR-X FDIV.S")
    # Integer MAC / divide. A 16x16 multiplier is cheap enough that 2 cycles
    # (the base MUL's cost) is the natural default; divide is iterative.
    intMulCycles = Param.Unsigned(2, "Cycles for AVR-X IMUL.*")
    intDivCycles = Param.Unsigned(12, "Cycles for AVR-X IDIVMOD.*")

    ArchDecoder = AVRDecoder
    ArchMMU = AVRMMU
    ArchInterrupts = AVRInterrupts
    ArchISA = AVRISA

    mmu = AVRMMU()
