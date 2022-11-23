
#pragma once

#include <cstdint>
#include <cstring>

#include "../Common/ByteSwap.h"

#include "BogusBoard.h"

namespace Debug
{
	void Halt(const char* text, ...);
	void Report(const char* text, ...);
}
