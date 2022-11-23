
#pragma once

#include <cstdint>
#include <map>
#include <atomic>
#include <unordered_map>

#include "../Common/ByteSwap.h"
#include "../Common/Spinlock.h"
#include "../Common/Thread.h"
#include "../Common/Json.h"
#include "../Common/Jdi.h"

#include "../GekkoCore/GekkoCore.h"

#include "../BogusBoard/BogusBoard.h"

#include "MiniElf.h"
#include "LoadElf.h"

#define UnitTestOK 0x55aa
#define UnitTestFAIL 0xaa55
#define UnitTestCanaryAddress 0x100

namespace Gekko
{
	void SixtyBus_ReadWord(uint32_t phys_addr, uint32_t* reg);
}
