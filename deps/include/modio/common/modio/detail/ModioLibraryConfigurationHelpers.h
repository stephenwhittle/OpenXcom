#pragma once

// Unreal will also define MODIO_API, so we overwrite it, and restore it later
#if MODIO_PLATFORM_UNREAL
	#pragma warning(disable : 4005)
#endif

#ifdef MODIO_DLL
	#ifdef MODIO_BUILD_DLL
		/*Enabled as "export" while compiling the dll project*/
		#define DLLEXPORT __declspec(dllexport)
	#else
		/*Enabled as "import" in the Client side for using already created dll file*/
		#define DLLEXPORT __declspec(dllimport)
	#endif
#else
	#define DLLEXPORT
#endif


#ifndef MODIO_NO_HEADER_ONLY
	#define MODIO_API DLLEXPORT inline
#else
	#define MODIO_API
#endif

#if MODIO_UNREAL_PLATFORM
	#pragma warning(pop)
#endif