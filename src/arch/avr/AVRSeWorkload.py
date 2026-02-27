from m5.objects.Workload import SEWorkload


class AVRSEWorkload(SEWorkload):
    type = "AVRSEWorkload"
    cxx_header = "arch/avr/se_workload.hh"
    cxx_class = "gem5::AVR::SEWorkload"

    @classmethod
    def _is_compatible_with(cls, obj):
        return obj.get_arch() == "avr"
