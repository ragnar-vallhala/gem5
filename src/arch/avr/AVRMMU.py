from m5.objects.AVRTLB import AVRTLB
from m5.objects.BaseMMU import BaseMMU


class AVRMMU(BaseMMU):
    type = "AVRMMU"
    cxx_class = "gem5::AVRMMU"
    cxx_header = "arch/avr/avr_mmu.hh"

    itb = AVRTLB(entry_type="instruction")
    dtb = AVRTLB(entry_type="data")

    @classmethod
    def walkerPorts(cls):
        return []

    def connectWalkerPorts(self, iport, dport):
        pass
