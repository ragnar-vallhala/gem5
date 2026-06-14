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
# NOTE: 100kHz is a deliberate test-only setting. At 1 cycle/instruction this
# makes ITERATIONS=1 (~1.3M instructions) exceed CoreMark's 10-second runtime
# gate, so the benchmark reports a "valid" result quickly. CoreMark/MHz is
# clock-independent so it stays correct; the raw Iterations/Sec reflects this
# clock. Revert to a realistic clock once per-instruction cycle costs are added.
system.clk_domain.clock = "100kHz"
system.clk_domain.voltage_domain = VoltageDomain()

system.mem_mode = "atomic"
# Use a very large range to cover everything
system.mem_ranges = [AddrRange("128MB")]

system.cpu = AVRCPU()
# M1 memory-hierarchy study: external-memory wait states per data access,
# selectable via the AVR_DATA_WS env var (0 = validated internal-SRAM timing).
system.cpu.dataWaitStates = int(os.environ.get("AVR_DATA_WS", "0"))
system.membus = SystemXBar()

# Use icache_port and dcache_port
system.cpu.icache_port = system.membus.cpu_side_ports
system.cpu.dcache_port = system.membus.cpu_side_ports

system.mem_ctrl = SimpleMemory(range=system.mem_ranges[0], latency="1ns")
system.mem_ctrl.port = system.membus.mem_side_ports
system.system_port = system.membus.cpu_side_ports

system.workload = AVRSEWorkload()

# Binary path for CoreMark
binary = "programs/coremark/coremark.elf"

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
