from m5.objects.BaseInterrupts import BaseInterrupts


class AVRInterrupts(BaseInterrupts):
    type = "AVRInterrupts"
    cxx_class = "gem5::AVRInterrupts"
    cxx_header = "arch/avr/avr_interrupts.hh"
