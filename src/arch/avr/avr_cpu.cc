#include "arch/avr/avr_cpu.hh"
#include "params/AVRCPU.hh"
#include "mem/packet.hh"
#include "sim/system.hh"
#include "base/logging.hh"
#include <iostream>

namespace gem5
{

    AVRCPU::AVRCPU(const AVRCPUParams &p)
        : BaseCPU(p),
          instPort(name() + ".inst_port", this),
          dataPort(name() + ".data_port", this),
          ArchMMU(p.ArchMMU),
          ArchInterrupts(p.ArchInterrupts),
          insts(0),
          ops(0)
    {
        if (!ArchInterrupts)
        {
            warn("AVRCPU: No valid AVRInterrupts object bound to CPU.\n");
        }
    }

    /********************* CPU EXECUTION ************************/

    void AVRCPU::executeInstruction()
    {
        ThreadContext *tc = threadContexts[0];

        Addr pc = tc->pcState().instAddr();

        // Fetch 1 byte from memory
        uint8_t instr = system->physProxy.read<uint8_t>(pc);

        std::cout << "[AVR] PC=" << std::hex << pc
                  << " INSTR=0x" << (int)instr << std::dec << std::endl;

        insts++;
        ops++;

        // For now increment PC by 1 byte
        tc->pcState(pc + 1);
    }

    void AVRCPU::tick()
    {
        executeInstruction();
    }

    /********************* INSTRUCTION PORT ************************/

    Tick AVRCPU::AVRInstPort::recvAtomic(PacketPtr pkt)
    {
        // 1 cycle latency for instruction fetches
        return 1;
    }

    /********************* DATA PORT ************************/

    Tick AVRCPU::AVRDataPort::recvAtomic(PacketPtr pkt)
    {
        // 1 cycle latency for loads/stores
        return 1;
    }
    AddrRangeList AVRCPU::AVRInstPort::getAddrRanges() const
    {
        // Example: AVR ATmega328P has 32KB flash
        AddrRangeList ranges;
        ranges.push_back(RangeSize(0x0000, 32 * 1024));
        return ranges;
    }
    AddrRangeList AVRCPU::AVRDataPort::getAddrRanges() const
    {
        AddrRangeList ranges;

        // Example ranges:
        // 0x0000–0x08FF : SRAM (2KB)
        // 0x2000–0x20FF : IO (256B)

        ranges.push_back(RangeSize(0x0000, 2048)); // SRAM
        ranges.push_back(RangeSize(0x2000, 256));  // I/O registers

        return ranges;
    }

} // namespace gem5
