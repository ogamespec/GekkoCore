
#include "pch.h"

uint8_t* RAM;
uint8_t* BootROM;

void HWInit()
{
	RAM = new uint8_t[RAM_SIZE];
	memset(RAM, 0, RAM_SIZE);

	BootROM = new uint8_t[BOOTROM_SIZE];
	memset(BootROM, 0xff, BOOTROM_SIZE);
}

void HWShutdown()
{
	delete[] RAM;
	delete[] BootROM;
}

namespace Gekko
{
	void SixtyBus_ReadByte(uint32_t phys_addr, uint32_t* reg)
	{
		uint8_t* ptr;

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

		if (phys_addr < RAM_SIZE)
		{
			ptr = &RAM[phys_addr];
			*ptr = (uint8_t)data;
		}
	}

	void SixtyBus_ReadHalf(uint32_t phys_addr, uint32_t* reg)
	{
		uint8_t* ptr;

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

		if (phys_addr < RAM_SIZE)
		{
			ptr = &RAM[phys_addr];
			*(uint16_t*)ptr = _BYTESWAP_UINT16((uint16_t)data);
		}
	}

	void SixtyBus_ReadWord(uint32_t phys_addr, uint32_t* reg)
	{
		uint8_t* ptr;

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

		if (phys_addr < RAM_SIZE)
		{
			ptr = &RAM[phys_addr];
			*(uint32_t*)ptr = _BYTESWAP_UINT32(data);
		}
	}

	void SixtyBus_ReadDouble(uint32_t phys_addr, uint64_t* reg)
	{
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
		if (phys_addr < RAM_SIZE)
		{
			uint8_t* buf = &RAM[phys_addr];
			*(uint64_t*)buf = _BYTESWAP_UINT64(*data);
		}
	}

	void SixtyBus_ReadBurst(uint32_t phys_addr, uint8_t burstData[BURST_SIZE])
	{
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
		if ((phys_addr + BURST_SIZE) <= RAM_SIZE)
		{
			memcpy(&RAM[phys_addr], burstData, BURST_SIZE);
		}
	}
}
