#pragma once
#if defined(__cplusplus) && __cplusplus >= 201703L && defined(__has_include)
#if __has_include(<filesystem>)
#define MODIO_USE_STD_FS
#include <filesystem>
namespace Modio
{
	namespace filesystem = std::filesystem;
}
#endif
#endif

#ifndef MODIO_USE_STD_FS

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include "ghc/filesystem.hpp"
#undef WIN32_LEAN_AND_MEAN
#else
#include "ghc/filesystem.hpp"
#endif

namespace Modio
{
	namespace filesystem = ghc::filesystem;
}
#endif
