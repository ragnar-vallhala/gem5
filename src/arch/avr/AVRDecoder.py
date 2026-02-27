from m5.objects.InstDecoder import InstDecoder


class AVRDecoder(InstDecoder):
    type = "AVRDecoder"
    cxx_class = "gem5::AVRISAInst::Decoder"
    cxx_header = "arch/avr/decoder.hh"
    # You will plug your decode table here
    pass
