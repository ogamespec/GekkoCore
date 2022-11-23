
#include "pch.h"

uint8_t* RAM;
uint8_t* BootROM;

void HWInit()
{
	Debug::Report("HWInit\n");

	RAM = new uint8_t[RAM_SIZE];
	memset(RAM, 0, RAM_SIZE);

	BootROM = new uint8_t[BOOTROM_SIZE];
	memset(BootROM, 0xff, BOOTROM_SIZE);
}

void HWShutdown()
{
	Debug::Report("HWShutdown\n");

	delete[] RAM;
	delete[] BootROM;
}

namespace Gekko
{
	void SixtyBus_ReadByte(uint32_t phys_addr, uint32_t* reg)
	{
		uint8_t* ptr;

		Debug::Report("60x Read Byte: 0x%08X\n", phys_addr);

		if (phys_addr >= BOOTROM_START_ADDRESS)
		{
			ptr = &BootROM[phys_addr - BOOTROM_START_ADDRESS];
			*reg = (uint32_t)*ptr;
			return;
		}
		else if (phys_addr < RAM_SIZE)
		{
			ptr = &RAM[phys_addr];
			*reg = (uint32_t)*ptr;
		}
		else
		{
			*reg = UNMAPPED_VALUE;
		}
	}

	void SixtyBus_WriteByte(uint32_t phys_addr, uint32_t data)
	{
		uint8_t* ptr;

		Debug::Report("60x Write Byte: 0x%08X = 0x%02X\n", phys_addr, (uint8_t)data);

		if (phys_addr < RAM_SIZE)
		{
			ptr = &RAM[phys_addr];
			*ptr = (uint8_t)data;
		}
	}

	void SixtyBus_ReadHalf(uint32_t phys_addr, uint32_t* reg)
	{
		uint8_t* ptr;

		Debug::Report("60x Read UInt16: 0x%08X\n", phys_addr);

		if (phys_addr >= BOOTROM_START_ADDRESS)
		{
			ptr = &BootROM[phys_addr - BOOTROM_START_ADDRESS];
			*reg = (uint32_t)_BYTESWAP_UINT16(*(uint16_t*)ptr);
		}
		else if (phys_addr < RAM_SIZE)
		{
			ptr = &RAM[phys_addr];
			*reg = (uint32_t)_BYTESWAP_UINT16(*(uint16_t*)ptr);
		}
		else
		{
			*reg = UNMAPPED_VALUE;
		}
	}

	void SixtyBus_WriteHalf(uint32_t phys_addr, uint32_t data)
	{
		uint8_t* ptr;

		Debug::Report("60x Write UInt16: 0x%08X = 0x%04X\n", phys_addr, (uint16_t)data);

		if (phys_addr < RAM_SIZE)
		{
			ptr = &RAM[phys_addr];
			*(uint16_t*)ptr = _BYTESWAP_UINT16((uint16_t)data);
		}
	}

	void SixtyBus_ReadWord(uint32_t phys_addr, uint32_t* reg)
	{
		uint8_t* ptr;

		Debug::Report("60x Read UInt32: 0x%08X\n", phys_addr);

		if (phys_addr < RAM_SIZE)
		{
			ptr = &RAM[phys_addr];
			*reg = _BYTESWAP_UINT32(*(uint32_t*)ptr);
		}
		else if (phys_addr >= BOOTROM_START_ADDRESS)
		{
			ptr = &BootROM[phys_addr - BOOTROM_START_ADDRESS];
			*reg = _BYTESWAP_UINT32(*(uint32_t*)ptr);
		}
		else
		{
			*reg = UNMAPPED_VALUE;
		}
	}

	void SixtyBus_WriteWord(uint32_t phys_addr, uint32_t data)
	{
		uint8_t* ptr;

		Debug::Report("60x Write UInt32: 0x%08X = 0x%08X\n", phys_addr, data);

		if (phys_addr < RAM_SIZE)
		{
			ptr = &RAM[phys_addr];
			*(uint32_t*)ptr = _BYTESWAP_UINT32(data);
		}
	}

	void SixtyBus_ReadDouble(uint32_t phys_addr, uint64_t* reg)
	{
		Debug::Report("60x Read UInt64: 0x%08X\n", phys_addr);

		if (phys_addr < RAM_SIZE)
		{
			uint8_t* buf = &RAM[phys_addr];
			*reg = _BYTESWAP_UINT64(*(uint64_t*)buf);
		}
		else
		{
			*reg = UNMAPPED_VALUE;
		}
	}

	void SixtyBus_WriteDouble(uint32_t phys_addr, uint64_t* data)
	{
		Debug::Report("60x Write UInt64: 0x%08X = 0x%llX\n", phys_addr, *data);

		if (phys_addr < RAM_SIZE)
		{
			uint8_t* buf = &RAM[phys_addr];
			*(uint64_t*)buf = _BYTESWAP_UINT64(*data);
		}
	}

	void SixtyBus_ReadBurst(uint32_t phys_addr, uint8_t burstData[BURST_SIZE])
	{
		Debug::Report("60x Read Burst: 0x%08X\n", phys_addr);

		if ((phys_addr + BURST_SIZE) <= RAM_SIZE)
		{
			memcpy(burstData, &RAM[phys_addr], BURST_SIZE);
		}
		else
		{
			memset(burstData, UNMAPPED_VALUE, BURST_SIZE);
		}
	}

	void SixtyBus_WriteBurst(uint32_t phys_addr, uint8_t burstData[BURST_SIZE])
	{
		Debug::Report("60x Write Burst: 0x%08X\n", phys_addr);

		if ((phys_addr + BURST_SIZE) <= RAM_SIZE)
		{
			memcpy(&RAM[phys_addr], burstData, BURST_SIZE);
		}
	}
}
