#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <cassert>
#include <atomic>
#include <cmath>
#include <cstring>
#include <unordered_map>

#include "../Common/ByteSwap.h"
#include "../Common/Spinlock.h"
#include "../Common/Thread.h"
#include "../Common/Json.h"
#include "../Common/Jdi.h"

#include "Config.h"
#if GEKKOCORE_JDI
#include "GekkoCommands.h"
#endif
#include "GekkoCore.h"
#include "GekkoDecoder.h"
#include "Interpreter.h"
#include "GekkoDisasm.h"
#include "GekkoAssembler.h"
#if GEKKOCORE_USE_JITC
#include "Jitc.h"
#endif
#include "TLB.h"
#include "Cache.h"
#include "SixtyBus.h"

#ifdef _LINUX
#define _stricmp strcasecmp
#endif
