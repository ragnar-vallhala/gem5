from m5.objects.BaseTLB import BaseTLB
from m5.objects.ClockedObject import ClockedObject
from m5.params import *
from m5.proxy import *


class AVRTLB(BaseTLB):
    type = "AVRTLB"
    cxx_class = "gem5::AVRTLB"
    cxx_header = "arch/avr/avr_tlb.hh"

    size = Param.Int(64, "TLB size")
