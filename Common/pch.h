#pragma once

#include <vector>
#include <list>
#include <map>
#include <string>
#include <cassert>
#include <atomic>

#ifdef _LINUX
#include <memory.h>
#include <string.h>
#include <unistd.h>		// usleep
#include <pthread.h>
#include <signal.h>
#include <libgen.h>		// dirname / basename
#include <sys/types.h>
#include <sys/stat.h>	// _wstat (IsDirectory)
#include <dirent.h>		// BuildFileTree
#define _stricmp strcasecmp
#endif

#include "Spinlock.h"
#include "Thread.h"
#include "Json.h"
#include "Jdi.h"
#include "String.h"
#include "File.h"

//#include "../Debugger/Report.h"
