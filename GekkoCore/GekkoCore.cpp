// CPU controls 
#include "pch.h"

namespace Gekko
{
    // The main driving force behind the entire emulator. All other threads are based on changing the TBR Gekko register.
    void GekkoCore::GekkoThreadProc(void* Parameter)
    {
        GekkoCore* core = (GekkoCore*)Parameter;

        if (core->suspended)
        {
            Thread::Sleep(50);
            return;
        }

        if (core->EnableTestBreakpoints)
        {
            core->TestBreakpoints();
        }

#if GEKKOCORE_USE_JITC
        core->jitc->Execute();
#else
        core->interp->ExecuteOpcode();
#endif
    }

    GekkoCore::GekkoCore()
    {
        cache = new Cache(this);

        gatherBuffer = new GatherBuffer(this);

        interp = new Interpreter(this);

#if GEKKOCORE_USE_JITC
        jitc = new Jitc(this);
#endif

        gekkoThread = new Thread(GekkoThreadProc, false, this, "GekkoCore");

        Reset();
    }

    GekkoCore::~GekkoCore()
    {
        StopOpcodeStatsThread();
        delete gekkoThread;
        delete interp;
#if GEKKOCORE_USE_JITC
        delete jitc;
#endif
        delete gatherBuffer;
    }

    // Reset processor
    void GekkoCore::Reset()
    {
        one_second = CPU_TIMER_CLOCK;
        intFlag = false;
        ops = 0;

        // TODO: Make switchable
        //EffectiveToPhysical = &GekkoCore::EffectiveToPhysicalNoMmu;

        // BAT registers are scattered across the SPR address space. This is not very convenient, we will make it convenient.

        dbatu[0] = &regs.spr[SPR::DBAT0U];
        dbatu[1] = &regs.spr[SPR::DBAT1U];
        dbatu[2] = &regs.spr[SPR::DBAT2U];
        dbatu[3] = &regs.spr[SPR::DBAT3U];

        dbatl[0] = &regs.spr[SPR::DBAT0L];
        dbatl[1] = &regs.spr[SPR::DBAT1L];
        dbatl[2] = &regs.spr[SPR::DBAT2L];
        dbatl[3] = &regs.spr[SPR::DBAT3L];

        ibatu[0] = &regs.spr[SPR::IBAT0U];
        ibatu[1] = &regs.spr[SPR::IBAT1U];
        ibatu[2] = &regs.spr[SPR::IBAT2U];
        ibatu[3] = &regs.spr[SPR::IBAT3U];

        ibatl[0] = &regs.spr[SPR::IBAT0L];
        ibatl[1] = &regs.spr[SPR::IBAT1L];
        ibatl[2] = &regs.spr[SPR::IBAT2L];
        ibatl[3] = &regs.spr[SPR::IBAT3L];

        // Registers

        memset(&regs, 0, sizeof(regs));

        // Disable translation for now
        regs.msr &= ~(MSR_DR | MSR_IR);

        regs.tb.uval = 0;
        regs.spr[SPR::HID1] = 0x8000'0000;
        regs.spr[SPR::DEC] = 0;
        regs.spr[SPR::CTR] = 0;

        gatherBuffer->Reset();

#if GEKKOCORE_USE_JITC
        jitc->Reset();
#endif
        ResetCompiledSegmentsCount();
        ResetExecutedSegmentsCount();

        dtlb.InvalidateAll();
        itlb.InvalidateAll();
        cache->Reset();
        Exception(Exception::EXCEPTION_SYSTEM_RESET);
    }

    // Modify CPU counters
    void GekkoCore::Tick()
    {
        regs.tb.uval += CounterStep;         // timer

        uint32_t old = regs.spr[SPR::DEC];
        regs.spr[SPR::DEC]--;          // decrementer
        if ((old ^ regs.spr[SPR::DEC]) & 0x80000000)
        {
            if (regs.msr & MSR_EE)
            {
                decreq = 1;
                Report("decrementer exception (OS alarm), pc:%08X\n", regs.pc);
            }
        }
    }

    int64_t GekkoCore::GetTicks()
    {
        return regs.tb.sval;
    }

    // 1 second of emulated CPU time.
    int64_t GekkoCore::OneSecond()
    {
        return one_second;
    }

    // Swap longs (no need in assembly, used by tools)
    void GekkoCore::SwapArea(uint32_t* addr, int count)
    {
        uint32_t* until = addr + count / sizeof(uint32_t);

        while (addr != until)
        {
            *addr = _BYTESWAP_UINT32(*addr);
            addr++;
        }
    }

    // Swap shorts (no need in assembly, used by tools)
    void GekkoCore::SwapAreaHalf(uint16_t* addr, int count)
    {
        uint16_t* until = addr + count / sizeof(uint16_t);

        while (addr != until)
        {
            *addr = _BYTESWAP_UINT16(*addr);
            addr++;
        }
    }

    void GekkoCore::Step()
    {
        interp->ExecuteOpcode();
    }

    void GekkoCore::StepVerbose()
    {
        uint32_t old_pc = regs.pc;
        Report("0x%08X:\n", old_pc);
        interp->ExecuteOpcode();
        interp->DumpDecoderInfo(old_pc);
    }

    void GekkoCore::AssertInterrupt()
    {
        intFlag = true;
    }

    void GekkoCore::ClearInterrupt()
    {
        intFlag = false;
    }

    void GekkoCore::Exception(Gekko::Exception code)
    {
        //DBReport2(DbgChannel::CPU, "Gekko Exception: #%04X\n", (uint16_t)code);

        if (exception)
        {
            Halt("CPU Double Fault!\n");
        }

        // save regs

        regs.spr[Gekko::SPR::SRR0] = regs.pc;
        regs.spr[Gekko::SPR::SRR1] = regs.msr;

        // Special processing for MMU
        if (code == Exception::EXCEPTION_ISI)
        {
            regs.spr[Gekko::SPR::SRR1] &= 0x0fff'ffff;

            switch (MmuLastResult)
            {
                case MmuResult::PageFault:
                    regs.spr[Gekko::SPR::SRR1] |= 0x4000'0000;
                    break;

                case MmuResult::ProtectedFetch:
                    regs.spr[Gekko::SPR::SRR1] |= 0x0800'0000;
                    break;

                case MmuResult::NoExecute:
                    regs.spr[Gekko::SPR::SRR1] |= 0x1000'0000;
                    break;
                
                default:
                    break;
            }
        }
        else if (code == Exception::EXCEPTION_DSI)
        {
            regs.spr[Gekko::SPR::DSISR] = 0;

            switch (MmuLastResult)
            {
                case MmuResult::PageFault:
                    regs.spr[Gekko::SPR::DSISR] |= 0x4000'0000;
                    break;

                case MmuResult::ProtectedRead:
                    regs.spr[Gekko::SPR::DSISR] |= 0x0800'0000;
                    break;
                
                case MmuResult::ProtectedWrite:
                    regs.spr[Gekko::SPR::DSISR] |= 0x0A00'0000;
                    break;

                default:
                    break;
            }
        }

        // Special processing for Program
        if (code == Exception::EXCEPTION_PROGRAM)
        {
            regs.spr[Gekko::SPR::SRR1] &= 0x0000'ffff;

            switch (PrCause)
            {
                case PrivilegedCause::FpuEnabled:
                    regs.spr[Gekko::SPR::SRR1] |= 0x0010'0000;
                    break;
                case PrivilegedCause::IllegalInstruction:
                    regs.spr[Gekko::SPR::SRR1] |= 0x0008'0000;
                    break;
                case PrivilegedCause::Privileged:
                    regs.spr[Gekko::SPR::SRR1] |= 0x0004'0000;
                    break;
                case PrivilegedCause::Trap:
                    regs.spr[Gekko::SPR::SRR1] |= 0x0002'0000;
                    break;
                default:
                    break;
            }
        }

        // disable address translation
        regs.msr &= ~(MSR_IR | MSR_DR);

        regs.msr &= ~MSR_RI;

        regs.msr &= ~MSR_EE;

        // change PC and set exception flag
        regs.pc = (uint32_t)code;

        if (code == Exception::EXCEPTION_SYSTEM_RESET)
        {
            regs.pc |= 0xfff0'0000;
        }

        exception = true;
    }

    uint32_t GekkoCore::EffectiveToPhysical(uint32_t ea, MmuAccess type, int& WIMG)
    {
#if GEKKOCORE_SIMPLE_MMU
        return EffectiveToPhysicalNoMmu(ea, type, WIMG);
#else
        return EffectiveToPhysicalMmu(ea, type, WIMG);
#endif
    }

    void GekkoCore::Halt(const char* text, ...)
    {
        va_list arg;
        char buf[0x1000] = { 0, };

        va_start(arg, text);
        vsprintf_s(buf, sizeof(buf), text, arg);
        va_end(arg);

        Debug::Halt(buf);
    }

    void GekkoCore::Report(const char* text, ...)
    {
        va_list arg;
        char buf[0x1000] = { 0, };

        va_start(arg, text);
        vsprintf_s(buf, sizeof(buf), text, arg);
        va_end(arg);

        Debug::Report(buf);
    }
}
