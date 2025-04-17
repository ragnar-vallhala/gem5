from m5.params import *
from m5.proxy import *
from m5.SimObject import SimObject


class AVR(SimObject):
    type = "AVR"
    cxx_header = "arch/avr/isa.hh"
    cxx_class = "gem5::AVR"
