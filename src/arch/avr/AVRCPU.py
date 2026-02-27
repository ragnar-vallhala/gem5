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

    ArchDecoder = AVRDecoder
    ArchMMU = AVRMMU
    ArchInterrupts = AVRInterrupts
    ArchISA = AVRISA

    mmu = AVRMMU()
