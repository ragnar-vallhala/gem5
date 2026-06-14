import os

import m5
from m5.objects import (
    AVRCPU,
    AddrRange,
    AVRSEWorkload,
    Process,
    Root,
    SimpleMemory,
    SrcClockDomain,
    System,
    SystemXBar,
    VoltageDomain,
)

system = System()
system.clk_domain = SrcClockDomain()
system.clk_domain.clock = "1GHz"
system.clk_domain.voltage_domain = VoltageDomain()

system.mem_mode = "atomic"
# Use a very large range to cover everything
system.mem_ranges = [AddrRange("128MB")]

system.cpu = AVRCPU()
system.membus = SystemXBar()

# Use icache_port and dcache_port
system.cpu.icache_port = system.membus.cpu_side_ports
system.cpu.dcache_port = system.membus.cpu_side_ports

system.mem_ctrl = SimpleMemory(range=system.mem_ranges[0], latency="1ns")
system.mem_ctrl.port = system.membus.mem_side_ports
system.system_port = system.membus.cpu_side_ports

system.workload = AVRSEWorkload()

# Binary path for CoreMark
binary = "test_print.elf"

process = Process()
process.cmd = [binary]
system.cpu.workload = process
system.cpu.createThreads()
system.cpu.createInterruptController()

root = Root(full_system=False, system=system)
m5.instantiate()

print("Beginning CoreMark simulation!")
event = m5.simulate()
print(f"Exiting @ tick {m5.curTick()} because {event.getCause()}")

checkpoint_dir = os.path.join(m5.options.outdir, "final_state")
print(f"Saving final state to {checkpoint_dir}")
m5.checkpoint(checkpoint_dir)
