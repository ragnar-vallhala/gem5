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

    ArchDecoder = AVRDecoder
    ArchMMU = AVRMMU
    ArchInterrupts = AVRInterrupts
    ArchISA = AVRISA

    mmu = AVRMMU()
