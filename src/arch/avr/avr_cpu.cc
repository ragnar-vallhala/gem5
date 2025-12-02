#include "arch/avr/avr_cpu.hh"

// Forward-declare the auto-generated params struct name. SCons will generate
// AVRCPUParams for you from the Python SimObject at build time.
#include "params/AVRCPU.hh"

namespace gem5
{

    AVRCPU::AVRCPU(const AVRCPUParams &p)
        : BaseCPU(p),
          iport(this->name() + ".iport"),
          ArchMMU(p.ArchMMU),
          dport(this->name() + ".dport", this),
          ArchInterrupts(p.ArchInterrupts),
          insts(0), ops(0)
    {
        // Minimal constructor - nothing else needed
    }

    void
    AVRCPU::wakeup(ThreadID /*tid*/)
    {
        // Minimal stub: nothing to wake yet
    }

    void AVRCPU::executeInstruction()
    {
        // Use the first thread context
        ThreadContext *tc = threadContexts[0];

        // Get program counter
        Addr pc = tc->pcState().instAddr();

        // Create memory request
        RequestPtr req = std::make_shared<Request>(pc, 1, 0, 0); // size 1, flags 0
        PacketPtr pkt = new Packet(req, MemCmd::ReadReq);

        // Send atomic request
        Tick latency = iport.sendAtomic(pkt);

        // Read instruction byte
        uint8_t instr = *(pkt->getPtr<uint8_t>());

        // Update counters
        insts++;
        ops++;

        // Advance PC (for AVR, instructions are 1 or 2 bytes; here we use 1 byte for simplicity)
        tc->pcState(tc->pcState().instAddr() + 2);
    }

} // namespace gem5
