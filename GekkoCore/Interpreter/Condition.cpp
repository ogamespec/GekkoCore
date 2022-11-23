// Condition Register Logical Instructions
#include "../pch.h"
#include "InterpreterPrivate.h"

namespace Gekko
{

	// CR[crbd] = CR[crba] & CR[crbb]
	void Interpreter::crand()
	{
		size_t crbd = info.paramBits[0], crba = info.paramBits[1], crbb = info.paramBits[2];
		
		uint32_t a = (core->regs.cr >> (31 - crba)) & 1;
		uint32_t b = (core->regs.cr >> (31 - crbb)) & 1;
		uint32_t d = (a & b) << (31 - crbd);     // <- crop is here
		uint32_t m = ~(1 << (31 - crbd));
		core->regs.cr = (core->regs.cr & m) | d;
		core->regs.pc += 4;
	}

	// CR[crbd] = CR[crba] & ~CR[crbb]
	void Interpreter::crandc()
	{
		size_t crbd = info.paramBits[0], crba = info.paramBits[1], crbb = info.paramBits[2];

		uint32_t a = (core->regs.cr >> (31 - crba)) & 1;
		uint32_t b = (core->regs.cr >> (31 - crbb)) & 1;
		uint32_t d = (a & (~b)) << (31 - crbd);     // <- crop is here
		uint32_t m = ~(1 << (31 - crbd));
		core->regs.cr = (core->regs.cr & m) | d;
		core->regs.pc += 4;
	}

	// CR[crbd] = CR[crba] EQV CR[crbb]
	void Interpreter::creqv()
	{
		size_t crbd = info.paramBits[0], crba = info.paramBits[1], crbb = info.paramBits[2];

		uint32_t a = (core->regs.cr >> (31 - crba)) & 1;
		uint32_t b = (core->regs.cr >> (31 - crbb)) & 1;
		uint32_t d = (!(a ^ b)) << (31 - crbd);     // <- crop is here
		uint32_t m = ~(1 << (31 - crbd));
		core->regs.cr = (core->regs.cr & m) | d;
		core->regs.pc += 4;
	}

	// CR[crbd] = !(CR[crba] & CR[crbb])
	void Interpreter::crnand()
	{
		size_t crbd = info.paramBits[0], crba = info.paramBits[1], crbb = info.paramBits[2];

		uint32_t a = (core->regs.cr >> (31 - crba)) & 1;
		uint32_t b = (core->regs.cr >> (31 - crbb)) & 1;
		uint32_t d = (!(a & b)) << (31 - crbd);     // <- crop is here
		uint32_t m = ~(1 << (31 - crbd));
		core->regs.cr = (core->regs.cr & m) | d;
		core->regs.pc += 4;
	}

	// CR[crbd] = !(CR[crba] | CR[crbb])
	void Interpreter::crnor()
	{
		size_t crbd = info.paramBits[0], crba = info.paramBits[1], crbb = info.paramBits[2];

		uint32_t a = (core->regs.cr >> (31 - crba)) & 1;
		uint32_t b = (core->regs.cr >> (31 - crbb)) & 1;
		uint32_t d = (!(a | b)) << (31 - crbd);     // <- crop is here
		uint32_t m = ~(1 << (31 - crbd));
		core->regs.cr = (core->regs.cr & m) | d;
		core->regs.pc += 4;
	}

	// CR[crbd] = CR[crba] | CR[crbb]
	void Interpreter::cror()
	{
		size_t crbd = info.paramBits[0], crba = info.paramBits[1], crbb = info.paramBits[2];

		uint32_t a = (core->regs.cr >> (31 - crba)) & 1;
		uint32_t b = (core->regs.cr >> (31 - crbb)) & 1;
		uint32_t d = (a | b) << (31 - crbd);     // <- crop is here
		uint32_t m = ~(1 << (31 - crbd));
		core->regs.cr = (core->regs.cr & m) | d;
		core->regs.pc += 4;
	}

	// CR[crbd] = CR[crba] | ~CR[crbb]
	void Interpreter::crorc()
	{
		size_t crbd = info.paramBits[0], crba = info.paramBits[1], crbb = info.paramBits[2];

		uint32_t a = (core->regs.cr >> (31 - crba)) & 1;
		uint32_t b = (core->regs.cr >> (31 - crbb)) & 1;
		uint32_t d = (a | (~b)) << (31 - crbd);     // <- crop is here
		uint32_t m = ~(1 << (31 - crbd));
		core->regs.cr = (core->regs.cr & m) | d;
		core->regs.pc += 4;
	}

	// CR[crbd] = CR[crba] ^ CR[crbb]
	void Interpreter::crxor()
	{
		size_t crbd = info.paramBits[0], crba = info.paramBits[1], crbb = info.paramBits[2];

		uint32_t a = (core->regs.cr >> (31 - crba)) & 1;
		uint32_t b = (core->regs.cr >> (31 - crbb)) & 1;
		uint32_t d = (a ^ b) << (31 - crbd);     // <- crop is here
		uint32_t m = ~(1 << (31 - crbd));
		core->regs.cr = (core->regs.cr & m) | d;
		core->regs.pc += 4;
	}

	// CR[4*crfd .. 4*crfd + 3] = CR[4*crfs .. 4*crfs + 3]
	void Interpreter::mcrf()
	{
		size_t crfd = 4 * (7 - info.paramBits[0]), crfs = 4 * (7 - info.paramBits[1]);
		core->regs.cr = (core->regs.cr & (~(0xf << crfd))) | (((core->regs.cr >> crfs) & 0xf) << crfd);
		core->regs.pc += 4;
	}

}
