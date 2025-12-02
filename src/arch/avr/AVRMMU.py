# src/arch/avr/AVRMMU.py
from m5.objects.BaseMMU import BaseMMU


class AVRMMU(BaseMMU):
    type = "AVRMMU"
    cxx_class = "gem5::AVRMMU"
    cxx_header = "arch/avr/avr_mmu.hh"
