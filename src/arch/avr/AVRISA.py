from m5.objects.BaseISA import BaseISA


class AVRISA(BaseISA):
    type = "AVRISA"
    cxx_class = "gem5::AVR"
    cxx_header = "arch/avr/isa.hh"
