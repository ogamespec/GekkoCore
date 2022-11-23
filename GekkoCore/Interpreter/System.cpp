// System Instructions
#include "../pch.h"
#include "InterpreterPrivate.h"

namespace Gekko
{

	void Interpreter::eieio()
	{
		core->regs.pc += 4;
	}

	// instruction synchronize.
	void Interpreter::isync()
	{
		core->regs.pc += 4;
	}

	// ea = (ra | 0) + rb
	// RESERVE = 1
	// RESERVE_ADDR = physical(ea)
	// rd = MEM(ea, 4)
	void Interpreter::lwarx()
	{
		int WIMG;
		uint32_t ea = core->regs.gpr[info.paramBits[2]];
		if (info.paramBits[1]) ea += core->regs.gpr[info.paramBits[1]];
		core->RESERVE = true;
		core->RESERVE_ADDR = core->EffectiveToPhysical(ea, Gekko::MmuAccess::Read, WIMG);
		core->ReadWord(ea, &core->regs.gpr[info.paramBits[0]]);
		if (core->exception) return;
		core->regs.pc += 4;
	}

	// ea = (ra | 0) + rb
	// if RESERVE
	//      then
	//          MEM(ea, 4) = rs
	//          CR0 = 0b00 || 0b1 || XER[SO]
	//          RESERVE = 0
	//      else
	//          CR0 = 0b00 || 0b0 || XER[SO]
	void Interpreter::stwcx_d()
	{
		uint32_t ea = core->regs.gpr[info.paramBits[2]];
		if (info.paramBits[1]) ea += core->regs.gpr[info.paramBits[1]];

		core->regs.cr &= 0x0fffffff;

		if (core->RESERVE)
		{
			core->WriteWord(ea, core->regs.gpr[info.paramBits[0]]);
			if (core->exception) return;
			core->regs.cr |= GEKKO_CR0_EQ;
			core->RESERVE = false;
		}

		if (IS_XER_SO) core->regs.cr |= GEKKO_CR0_SO;
		core->regs.pc += 4;
	}

	void Interpreter::sync()
	{
		core->regs.pc += 4;
	}

	// return from exception
	void Interpreter::rfi()
	{
		core->regs.msr &= ~(0x87C0FF73 | 0x00040000);
		core->regs.msr |= core->regs.spr[SPR::SRR1] & 0x87C0FF73;
		core->regs.pc = core->regs.spr[SPR::SRR0] & ~3;
	}

	// syscall
	void Interpreter::sc()
	{
		// pseudo-branch (to resume from next instruction after 'rfi')
		core->regs.pc += 4;
		core->Exception(Gekko::Exception::EXCEPTION_SYSTEM_CALL);
	}

	void Interpreter::tw()
	{
		int32_t a = core->regs.gpr[info.paramBits[1]], b = core->regs.gpr[info.paramBits[2]];
		int32_t to = info.paramBits[0];

		if (((a < b) && (to & 0x10)) ||
			((a > b) && (to & 0x08)) ||
			((a == b) && (to & 0x04)) ||
			(((uint32_t)a < (uint32_t)b) && (to & 0x02)) ||
			(((uint32_t)a > (uint32_t)b) && (to & 0x01)))
		{
			// pseudo-branch (to resume from next instruction after 'rfi')
			core->regs.pc += 4;
			core->PrCause = PrivilegedCause::Trap;
			core->Exception(Gekko::Exception::EXCEPTION_PROGRAM);
		}
		else
		{
			core->regs.pc += 4;
		}
	}

	void Interpreter::twi()
	{
		int32_t a = core->regs.gpr[info.paramBits[1]], b = (int32_t)info.Imm.Signed;
		int32_t to = info.paramBits[0];

		if (((a < b) && (to & 0x10)) ||
			((a > b) && (to & 0x08)) ||
			((a == b) && (to & 0x04)) ||
			(((uint32_t)a < (uint32_t)b) && (to & 0x02)) ||
			(((uint32_t)a > (uint32_t)b) && (to & 0x01)))
		{
			// pseudo-branch (to resume from next instruction after 'rfi')
			core->regs.pc += 4;
			core->PrCause = PrivilegedCause::Trap;
			core->Exception(Gekko::Exception::EXCEPTION_PROGRAM);
		}
		else
		{
			core->regs.pc += 4;
		}
	}

	// CR[4 * crfD .. 4 * crfd + 3] = XER[0-3]
	// XER[0..3] = 0b0000
	void Interpreter::mcrxr()
	{
		uint32_t mask = 0xf0000000 >> (4 * info.paramBits[0]);
		core->regs.cr &= ~mask;
		core->regs.cr |= (core->regs.spr[SPR::XER] & 0xf0000000) >> (4 * info.paramBits[0]);
		core->regs.spr[SPR::XER] &= ~0xf0000000;
		core->regs.pc += 4;
	}

	// rd = cr
	void Interpreter::mfcr()
	{
		core->regs.gpr[info.paramBits[0]] = core->regs.cr;
		core->regs.pc += 4;
	}

	// rd = msr
	void Interpreter::mfmsr()
	{
		if (core->regs.msr & MSR_PR)
		{
			core->PrCause = PrivilegedCause::Privileged;
			core->Exception(Exception::EXCEPTION_PROGRAM);
			return;
		}

		core->regs.gpr[info.paramBits[0]] = core->regs.msr;
		core->regs.pc += 4;
	}

	// We do not support access rights to SPRs, since all applications on the emulated system are executed with OEA rights.
	// A detailed study of all SPRs in all modes is in Docs\HW\SPR.txt. If necessary, it will be possible to wind the rights properly.

	// rd = spr
	void Interpreter::mfspr()
	{
		int spr = info.paramBits[1];
		uint32_t value;

		switch (spr)
		{
			case SPR::WPAR:
				value = (core->regs.spr[spr] & ~0x1f) | (core->gatherBuffer->NotEmpty() ? 1 : 0);
				break;

			case SPR::HID1:
				// Gekko PLL_CFG = 0b1000
				value = 0x8000'0000;
				break;

			default:
				value = core->regs.spr[spr];
				break;
		}

		core->regs.gpr[info.paramBits[0]] = value;
		core->regs.pc += 4;
	}

	// rd = tbr
	void Interpreter::mftb()
	{
		int tbr = info.paramBits[1];

		if (tbr == (int)TBR::TBL)
		{
			core->regs.gpr[info.paramBits[0]] = core->regs.tb.Part.l;
		}
		else if (tbr == (int)TBR::TBU)
		{
			core->regs.gpr[info.paramBits[0]] = core->regs.tb.Part.u;
		}

		core->regs.pc += 4;
	}

	// mask = (4)CRM[0] || (4)CRM[1] || ... || (4)CRM[7]
	// CR = (rs & mask) | (CR & ~mask)
	void Interpreter::mtcrf()
	{
		uint32_t m, crm = info.paramBits[0], a, d = core->regs.gpr[info.paramBits[1]];

		for (int i = 0; i < 8; i++)
		{
			if ((crm >> i) & 1)
			{
				a = (d >> (i << 2)) & 0xf;
				m = (0xf << (i << 2));
				core->regs.cr = (core->regs.cr & ~m) | (a << (i << 2));
			}
		}
		core->regs.pc += 4;
	}

	// msr = rs
	void Interpreter::mtmsr()
	{
		if (core->regs.msr & MSR_PR)
		{
			core->PrCause = PrivilegedCause::Privileged;
			core->Exception(Exception::EXCEPTION_PROGRAM);
			return;
		}

		uint32_t oldMsr = core->regs.msr;
		core->regs.msr = core->regs.gpr[info.paramBits[0]];

		if ((oldMsr & MSR_IR) != (core->regs.msr & MSR_IR))
		{
			core->itlb.InvalidateAll();
		}

		if ((oldMsr & MSR_DR) != (core->regs.msr & MSR_DR))
		{
			core->dtlb.InvalidateAll();
		}

		core->regs.pc += 4;
	}

	// spr = rs
	void Interpreter::mtspr()
	{
		int spr = info.paramBits[0];

		// Diagnostic output when the BAT registers are changed.

		if (spr >= SPR::IBAT0U && spr <= SPR::DBAT3L)
		{
			static const char* bat[] = {
				"IBAT0U", "IBAT0L", "IBAT1U", "IBAT1L",
				"IBAT2U", "IBAT2L", "IBAT3U", "IBAT3L",
				"DBAT0U", "DBAT0L", "DBAT1U", "DBAT1L",
				"DBAT2U", "DBAT2L", "DBAT3U", "DBAT3L"
			};

			bool msr_ir = (core->regs.msr & MSR_IR) ? true : false;
			bool msr_dr = (core->regs.msr & MSR_DR) ? true : false;

			core->Report("%s <- %08X (IR:%i DR:%i pc:%08X)\n",
				bat[spr - SPR::IBAT0U], core->regs.gpr[info.paramBits[1]], msr_ir, msr_dr, core->regs.pc);
		}
		
		switch (spr)
		{
			// decrementer
			case SPR::DEC:
				//DBReport2(DbgChannel::CPU, "set decrementer (OS alarm) to %08X\n", RRS);
				break;

			// page table base
			case SPR::SDR1:
			{
				bool msr_ir = (core->regs.msr & MSR_IR) ? true : false;
				bool msr_dr = (core->regs.msr & MSR_DR) ? true : false;

				core->Report("SDR <- %08X (IR:%i DR:%i pc:%08X)\n",
					core->regs.gpr[info.paramBits[1]], msr_ir, msr_dr, core->regs.pc);

				core->dtlb.InvalidateAll();
				core->itlb.InvalidateAll();
			}
			break;

			case SPR::TBL:
				core->regs.tb.Part.l = core->regs.gpr[info.paramBits[1]];
				core->Report("Set TBL: 0x%08X\n", core->regs.tb.Part.l);
				break;
			case SPR::TBU:
				core->regs.tb.Part.u = core->regs.gpr[info.paramBits[1]];
				core->Report("Set TBU: 0x%08X\n", core->regs.tb.Part.u);
				break;

			// write gathering buffer
			case SPR::WPAR:
				// A mtspr to WPAR invalidates the data.
				core->gatherBuffer->Reset();
				break;

			case SPR::HID0:
			{
				uint32_t bits = core->regs.gpr[info.paramBits[1]];
				core->cache->Enable((bits & HID0_DCE) ? true : false);
				core->cache->Freeze((bits & HID0_DLOCK) ? true : false);
				if (bits & HID0_DCFI)
				{
					bits &= ~HID0_DCFI;

					// On a real system, after a global cache invalidation, the data still remains in the L2 cache.
					// We cannot afford global invalidation, as the L2 cache is not supported.

					core->Report("Data Cache Flash Invalidate\n");
				}
				if (bits & HID0_ICFI)
				{
					bits &= ~HID0_ICFI;
#if GEKKOCORE_USE_JITC
					core->jitc->Reset();
#endif

					core->Report("Instruction Cache Flash Invalidate\n");
				}

				core->regs.spr[spr] = bits;
				core->regs.pc += 4;
				return;
			}
			break;

			case SPR::HID1:
				// Read only
				core->regs.pc += 4;
				return;

			case SPR::HID2:
			{
				uint32_t bits = core->regs.gpr[info.paramBits[1]];
				core->cache->LockedEnable((bits & HID2_LCE) ? true : false);
			}
			break;

			// Locked cache DMA

			case SPR::DMAU:
				//DBReport2(DbgChannel::CPU, "DMAU: 0x%08X\n", RRS);
				break;
			case SPR::DMAL:
			{
				core->regs.spr[spr] = core->regs.gpr[info.paramBits[1]];
				//DBReport2(DbgChannel::CPU, "DMAL: 0x%08X\n", RRS);
				if (core->regs.spr[SPR::DMAL] & GEKKO_DMAL_DMA_T)
				{
					uint32_t maddr = core->regs.spr[SPR::DMAU] & GEKKO_DMAU_MEM_ADDR;
					uint32_t lcaddr = core->regs.spr[SPR::DMAL] & GEKKO_DMAL_LC_ADDR;
					size_t length = ((core->regs.spr[SPR::DMAU] & GEKKO_DMAU_DMA_LEN_U) << GEKKO_DMA_LEN_SHIFT) |
						((core->regs.spr[SPR::DMAL] >> GEKKO_DMA_LEN_SHIFT) & GEKKO_DMAL_DMA_LEN_L);
					if (length == 0) length = 128;
					if (core->cache->IsLockedEnable())
					{
						core->cache->LockedCacheDma(
							(core->regs.spr[SPR::DMAL] & GEKKO_DMAL_DMA_LD) ? true : false,
							maddr,
							lcaddr,
							length);
					}
				}

				// It makes no sense to implement such a small Queue. We make all transactions instant.

				core->regs.spr[spr] &= ~(GEKKO_DMAL_DMA_T | GEKKO_DMAL_DMA_F);
				core->regs.pc += 4;
				return;
			}
			break;

			case SPR::GQR0:
			case SPR::GQR1:
			case SPR::GQR2:
			case SPR::GQR3:
			case SPR::GQR4:
			case SPR::GQR5:
			case SPR::GQR6:
			case SPR::GQR7:
				// In the sense of Dolphin OS, registers GQR1-7 are constantly reloaded when switching threads via `OSLoadContext`.
				// GQR0 is always 0 and is not reloaded when switching threads.
				break;

			case SPR::IBAT0U:
			case SPR::IBAT0L:
			case SPR::IBAT1U:
			case SPR::IBAT1L:
			case SPR::IBAT2U:
			case SPR::IBAT2L:
			case SPR::IBAT3U:
			case SPR::IBAT3L:
				core->itlb.InvalidateAll();
				break;

			case SPR::DBAT0U:
			case SPR::DBAT0L:
			case SPR::DBAT1U:
			case SPR::DBAT1L:
			case SPR::DBAT2U:
			case SPR::DBAT2L:
			case SPR::DBAT3U:
			case SPR::DBAT3L:
				core->dtlb.InvalidateAll();
				break;
		}

		// default
		core->regs.spr[spr] = core->regs.gpr[info.paramBits[1]];
		core->regs.pc += 4;
	}

	void Interpreter::dcbf()
	{
		int WIMG;
		uint32_t ea = info.paramBits[0] ? core->regs.gpr[info.paramBits[0]] + core->regs.gpr[info.paramBits[1]] : core->regs.gpr[info.paramBits[1]];

		uint32_t pa = core->EffectiveToPhysical(ea, MmuAccess::Read, WIMG);
		if (pa != Gekko::BadAddress)
		{
			core->cache->Flush(pa);
		}
		else
		{
			core->regs.spr[Gekko::SPR::DAR] = ea;
			core->Exception(Exception::EXCEPTION_DSI);
			return;
		}
		core->regs.pc += 4;
	}

	void Interpreter::dcbi()
	{
		int WIMG;
		uint32_t ea = info.paramBits[0] ? core->regs.gpr[info.paramBits[0]] + core->regs.gpr[info.paramBits[1]] : core->regs.gpr[info.paramBits[1]];

		if (core->regs.msr & MSR_PR)
		{
			core->PrCause = PrivilegedCause::Privileged;
			core->Exception(Exception::EXCEPTION_PROGRAM);
			return;
		}

		uint32_t pa = core->EffectiveToPhysical(ea, MmuAccess::Write, WIMG);
		if (pa != Gekko::BadAddress)
		{
			core->cache->Invalidate(pa);
		}
		else
		{
			core->regs.spr[Gekko::SPR::DAR] = ea;
			core->Exception(Exception::EXCEPTION_DSI);
			return;
		}
		core->regs.pc += 4;
	}

	void Interpreter::dcbst()
	{
		int WIMG;
		uint32_t ea = info.paramBits[0] ? core->regs.gpr[info.paramBits[0]] + core->regs.gpr[info.paramBits[1]] : core->regs.gpr[info.paramBits[1]];

		uint32_t pa = core->EffectiveToPhysical(ea, MmuAccess::Read, WIMG);
		if (pa != Gekko::BadAddress)
		{
			core->cache->Store(pa);
		}
		else
		{
			core->regs.spr[Gekko::SPR::DAR] = ea;
			core->Exception(Exception::EXCEPTION_DSI);
			return;
		}
		core->regs.pc += 4;
	}

	void Interpreter::dcbt()
	{
		int WIMG;

		if (core->regs.spr[Gekko::SPR::HID0] & HID0_NOOPTI)
		{
			core->regs.pc += 4;
			return;
		}

		uint32_t ea = info.paramBits[0] ? core->regs.gpr[info.paramBits[0]] + core->regs.gpr[info.paramBits[1]] : core->regs.gpr[info.paramBits[1]];

		uint32_t pa = core->EffectiveToPhysical(ea, MmuAccess::Read, WIMG);
		if (pa != Gekko::BadAddress)
		{
			core->cache->Touch(pa);
		}
		core->regs.pc += 4;
	}

	void Interpreter::dcbtst()
	{
		int WIMG;

		if (core->regs.spr[Gekko::SPR::HID0] & HID0_NOOPTI)
		{
			core->regs.pc += 4;
			return;
		}

		uint32_t ea = info.paramBits[0] ? core->regs.gpr[info.paramBits[0]] + core->regs.gpr[info.paramBits[1]] : core->regs.gpr[info.paramBits[1]];

		// TouchForStore is also made architecturally as a Read operation so that the MMU does not set the "Changed" bit for PTE.

		uint32_t pa = core->EffectiveToPhysical(ea, MmuAccess::Read, WIMG);
		if (pa != Gekko::BadAddress)
		{
			core->cache->TouchForStore(pa);
		}
		core->regs.pc += 4;
	}

	void Interpreter::dcbz()
	{
		int WIMG;
		uint32_t ea = info.paramBits[0] ? core->regs.gpr[info.paramBits[0]] + core->regs.gpr[info.paramBits[1]] : core->regs.gpr[info.paramBits[1]];

		uint32_t pa = core->EffectiveToPhysical(ea, MmuAccess::Write, WIMG);
		if (pa != Gekko::BadAddress)
		{
			core->cache->Zero(pa);
		}
		else
		{
			core->regs.spr[Gekko::SPR::DAR] = ea;
			core->Exception(Exception::EXCEPTION_DSI);
			return;
		}
		core->regs.pc += 4;
	}

	// DCBZ_L is used for the alien Locked Cache address mapping mechanism.
	// For example, calling dcbz_l 0xE0000000 will make this address be associated with Locked Cache for subsequent Load/Store operations.
	// Locked Cache is saved in RAM by another alien mechanism (DMA).

	void Interpreter::dcbz_l()
	{
		int WIMG;

		if (!core->cache->IsLockedEnable())
		{
			core->PrCause = PrivilegedCause::IllegalInstruction;
			core->Exception(Exception::EXCEPTION_PROGRAM);
			return;
		}

		uint32_t ea = info.paramBits[0] ? core->regs.gpr[info.paramBits[0]] + core->regs.gpr[info.paramBits[1]] : core->regs.gpr[info.paramBits[1]];

		uint32_t pa = core->EffectiveToPhysical(ea, MmuAccess::Write, WIMG);
		if (pa != Gekko::BadAddress)
		{
			core->cache->ZeroLocked(pa);
		}
		else
		{
			core->regs.spr[Gekko::SPR::DAR] = ea;
			core->Exception(Exception::EXCEPTION_DSI);
			return;
		}
		core->regs.pc += 4;
	}

	// Used as a hint to JITC so that it can invalidate the compiled code at this address.

	void Interpreter::icbi()
	{
		uint32_t address = info.paramBits[0] ? core->regs.gpr[info.paramBits[0]] + core->regs.gpr[info.paramBits[1]] : core->regs.gpr[info.paramBits[0]];
		address &= ~0x1f;

		int WIMG;
		uint32_t physicalAddress = core->EffectiveToPhysical(address, MmuAccess::Execute, WIMG);

#if GEKKOCORE_USE_JITC
		if (physicalAddress != BadAddress)
		{
			core->jitc->Invalidate(physicalAddress, 32);
		}
#endif
		
		core->regs.pc += 4;
	}

	// rd = sr[a]
	void Interpreter::mfsr()
	{
		if (core->regs.msr & MSR_PR)
		{
			core->PrCause = PrivilegedCause::Privileged;
			core->Exception(Exception::EXCEPTION_PROGRAM);
			return;
		}

		core->regs.gpr[info.paramBits[0]] = core->regs.sr[info.paramBits[1]];
		core->regs.pc += 4;
	}

	// rd = sr[rb]
	void Interpreter::mfsrin()
	{
		if (core->regs.msr & MSR_PR)
		{
			core->PrCause = PrivilegedCause::Privileged;
			core->Exception(Exception::EXCEPTION_PROGRAM);
			return;
		}

		core->regs.gpr[info.paramBits[0]] = core->regs.sr[core->regs.gpr[info.paramBits[1]] >> 28];
		core->regs.pc += 4;
	}

	// sr[a] = rs
	void Interpreter::mtsr()
	{
		if (core->regs.msr & MSR_PR)
		{
			core->PrCause = PrivilegedCause::Privileged;
			core->Exception(Exception::EXCEPTION_PROGRAM);
			return;
		}

		core->regs.sr[info.paramBits[0]] = core->regs.gpr[info.paramBits[1]];
		core->regs.pc += 4;
	}

	// sr[rb] = rs
	void Interpreter::mtsrin()
	{
		if (core->regs.msr & MSR_PR)
		{
			core->PrCause = PrivilegedCause::Privileged;
			core->Exception(Exception::EXCEPTION_PROGRAM);
			return;
		}

		core->regs.sr[core->regs.gpr[info.paramBits[1]] >> 28] = core->regs.gpr[info.paramBits[0]];
		core->regs.pc += 4;
	}

	void Interpreter::tlbie()
	{
		core->dtlb.Invalidate(core->regs.gpr[info.paramBits[0]]);
		core->itlb.Invalidate(core->regs.gpr[info.paramBits[0]]);
		core->regs.pc += 4;
	}

	void Interpreter::tlbsync()
	{
		core->regs.pc += 4;
	}

	void Interpreter::eciwx()
	{
		core->Halt("eciwx\n");
	}

	void Interpreter::ecowx()
	{
		core->Halt("ecowx\n");
	}

}
