#pragma once

#ifdef MODIO_PLATFORM_NO_LSTAT
#include <sys/stat.h>
extern "C" inline int lstat(const char* path, struct stat* outStat)
{
	return ::stat(path, outStat);
}

#endif

#ifdef MODIO_PLATFORM_UNREAL
	#if PLATFORM_WINDOWS
		#include "Windows/AllowWindowsPlatformTypes.h"
		#include "Windows/PreWindowsApi.h"
		#include "Windows/AllowWindowsPlatformAtomics.h"
	#endif
	#pragma warning(push)
	#pragma warning(disable : 4191)
	#include "ghc/filesystem.hpp"
	#pragma warning(pop)

	#if PLATFORM_WINDOWS
		#include "Windows/PostWindowsApi.h"
		#include "Windows/HideWindowsPlatformTypes.h"
		#include "Windows/HideWindowsPlatformAtomics.h"
	#endif
#else // _WIN32
	#include "ghc/filesystem.hpp"
#endif //_WIN32