# avr-gdb differential-test harness for the gem5 AVR port.
#
# Runs the SAME elf on avr-gdb's built-in AVR simulator (a known-good
# reference) and logs a per-instruction PC (and optionally register) trace.
# Diff this against a gem5 PC/register trace to find the first diverging
# instruction -- this is how the icall/ijmp, cpse, half-carry and adc-carry
# bugs were found.
#
# Usage:
#   avr-gdb -batch -x util/avr_diff_test.py
# then diff /tmp/sim_pc.log against a gem5 trace.
#
# Gotchas baked in below:
#   * avr-gdb's built-in sim tolerates SP > 2KB (flat memory), so the gem5
#     __stack=0x4fff CoreMark elf runs unmodified.
#   * The barebones port does I/O via the `break` instruction (0x9598):
#     R16=1 putchar(R17), R16=2 exit, R16=3 get_time. stepi does NOT advance
#     past a break, so we emulate it and set $pc manually.
#   * avr-gdb reads $pc as a byte address but WRITES it as a word address
#     (the write is doubled on read-back), so advance via (pc+2)>>1.
#   * get_time is forced to 0 so timing differences don't cause false
#     divergence (CoreMark control flow is timing-independent with a fixed
#     ITERATIONS); this makes only the final time-report formatting differ.
import gdb

ELF = "programs/coremark/coremark.elf"
MAX_STEPS = 1500000
LOG = "/tmp/sim_pc.log"

gdb.execute("file " + ELF)
gdb.execute("target sim")
gdb.execute("load")
gdb.execute("break *0x0")
gdb.execute("run")
inf = gdb.selected_inferior()
f = open(LOG, "w")
for i in range(MAX_STEPS):
    fr = gdb.selected_frame()
    pc = int(fr.read_register("pc")) & 0xFFFFFF
    f.write("%x\n" % pc)
    op = inf.read_memory(pc, 2)
    op = op[0][0] | (op[1][0] << 8)
    if op == 0x9598:  # break = syscall
        r16 = int(fr.read_register("r16")) & 0xFF
        if r16 == 2:
            f.write("EXIT\n")
            break
        if r16 == 3:  # get_time -> 0
            for r in (22, 23, 24, 25):
                gdb.execute("set $r%d = 0" % r)
        gdb.execute("set $pc = 0x%x" % ((pc + 2) >> 1))
    else:
        gdb.execute("stepi", to_string=True)
    if i % 20000 == 0:
        f.flush()
f.flush()
f.close()
print("SIM DONE at step", i)
gdb.execute("quit")
