// Configures the library in a header only fashion so developers can just drop in the library in their project
#pragma once

// We have custom DEBUG_RELEASE flags, as in unreal, we can't define DEBUG/_DEBUG as
// unreal has modified the allocator in debug, so we can't allocate all our objects with
// the standard allocator
#ifndef MODIO_DEBUG
	#if defined(DEBUG) || defined(_DEBUG)
		#define MODIO_DEBUG 1
	#else
		#define MODIO_DEBUG 0
	#endif
#endif

#ifndef MODIO_RELEASE
	#if defined(NDEBUG)
		#define MODIO_RELEASE 1
	#else
		#define MODIO_RELEASE 0
	#endif
#endif

// If we should track how many of each operation there are alive at all time
// useful if we want to find leaking operations and zombie operations
#ifndef MODIO_TRACK_OPS
	#define MODIO_TRACK_OPS 0
#endif

// BEGIN ASIO DEFINES
#ifndef ASIO_STANDALONE
	#define MODIO_DEFINED_ASIO_STANDALONE
	#define ASIO_STANDALONE
#endif

#ifndef ASIO_HAS_STD_ADDRESSOF
	#define MODIO_DEFINED_ASIO_HAS_STD_ADDRESSOF
	#define ASIO_HAS_STD_ADDRESSOF
#endif

/*#ifndef ASIO_NO_EXCEPTIONS
	#define MODIO_DEFINED_ASIO_NO_EXCEPTIONS
	#define ASIO_NO_EXCEPTIONS
#endif*/

#ifndef ASIO_HAS_STD_ARRAY
	#define MODIO_DEFINED_ASIO_HAS_STD_ARRAY
	#define ASIO_HAS_STD_ARRAY
#endif

#ifndef ASIO_HAS_CSTDINT
	#define MODIO_DEFINED_ASIO_HAS_CSTDINT
	#define ASIO_HAS_CSTDINT
#endif

#ifndef ASIO_HAS_STD_SHARED_PTR
	#define MODIO_DEFINED_ASIO_HAS_STD_SHARED_PTR
	#define ASIO_HAS_STD_SHARED_PTR
#endif

#ifndef ASIO_HAS_STD_TYPE_TRAITS
	#define MODIO_DEFINED_ASIO_HAS_STD_TYPE_TRAITS
	#define ASIO_HAS_STD_TYPE_TRAITS
#endif

#ifndef ASIO_HAS_VARIADIC_TEMPLATES
	#define MODIO_DEFINED_ASIO_HAS_VARIADIC_TEMPLATES
	#define ASIO_HAS_VARIADIC_TEMPLATES
#endif

#ifndef ASIO_HAS_STD_FUNCTION
	#define MODIO_DEFINED_ASIO_HAS_STD_FUNCTION
	#define ASIO_HAS_STD_FUNCTION
#endif

#ifndef ASIO_HAS_STD_CHRONO
	#define MODIO_DEFINED_ASIO_HAS_STD_CHRONO
	#define ASIO_HAS_STD_CHRONO
#endif

#ifndef ASIO_HAS_MOVE
	#define MODIO_DEFINED_ASIO_HAS_MOVE
	#define ASIO_HAS_MOVE
#endif

#ifndef ASIO_NO_DEFAULT_LINKED_LIBS
	#define MODIO_DEFINED_ASIO_NO_DEFAULT_LINKED_LIBS
	#define ASIO_NO_DEFAULT_LINKED_LIBS
#endif

#ifndef ASIO_DISABLE_IOCP
	#define MODIO_DEFINED_ASIO_DISABLE_IOCP
	#define ASIO_DISABLE_IOCP
#endif

#ifndef ASIO_NO_DEPRECATED
	#define MODIO_DEFINED_ASIO_NO_DEPRECATED
	#define ASIO_NO_DEPRECATED
#endif

#ifndef ASIO_NO_DEPRECATED
	#define MODIO_DEFINED_ASIO_NO_DEPRECATED
	#define ASIO_NO_DEPRECATED
#endif

#ifndef BOOST_ALL_NO_LIB
	#define MODIO_DEFINED_BOOST_ALL_NO_LIB
	#define BOOST_ALL_NO_LIB
#endif
// END ASIO DEFINES

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
	#define MODIO_DEFINED_WIN32_LEAN_AND_MEAN
#endif

#ifdef _WIN32
	#define NOCRYPT
	// Set the proper SDK version before including asio
	#include <SDKDDKVer.h>
	// Note asio includes Windows.h.
	#include <asio.hpp>
	// Ensure that we are linking against Winhttp that we require on Windows
	#pragma comment(lib, "Winhttp.lib")
#else // _WIN32
	#include <asio.hpp>
#endif //_WIN32

#ifndef FMT_HEADER_ONLY
	#define MODIO_DEFINED_FMT_HEADER_ONLY
	#define FMT_HEADER_ONLY
#endif

#ifndef BOOST_BEAST_DECL
	#define MODIO_DEFINED_BOOST_BEAST_DECL
	#define BOOST_BEAST_DECL inline
#endif

#ifndef NOMINMAX
	#define MODIO_DEFINED_NOMINMAX
	#define NOMINMAX
#endif

#ifndef D_UNICODE
	#define MODIO_DEFINED_D_UNICODE
	#define D_UNICODE
#endif

#ifndef UNICODE
	#define MODIO_DEFINED_UNICODE
	#define UNICODE
#endif
