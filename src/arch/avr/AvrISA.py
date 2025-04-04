from m5.params import *
from m5.proxy import *
from m5.SimObject import SimObject


class AvrISA(SimObject):
    type = "AvrISA"
    cxx_class = "gem5::AvrISA::ISA"
    cxx_header = "arch/avr/isa.hh"
