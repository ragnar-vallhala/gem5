from m5.objects.BaseISA import BaseISA


class AVRISA(BaseISA):
    instShift = 1  # AVR instructions are 16 bits
    # define your register file size here
