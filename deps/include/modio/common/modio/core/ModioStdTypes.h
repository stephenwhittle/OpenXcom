#pragma once
//These are necessary because we're pulling in windows.h via ghc fileystem
#include "ModioGeneratedVariables.h"
#include "modio/detail/ModioDefines.h"

#include "tl/optional.hpp"
#include <memory>
#include <system_error>
#include <chrono>
#include <cstdint>

namespace asio
{
	class mutable_buffer;
	class const_buffer;
} // namespace asio

namespace Modio
{
	template<typename Rep, typename Period = std::ratio<1>>
	using Duration = std::chrono::duration<Rep, Period>;

	using ErrorCode = std::error_code;

	/// @docpublic
	/// @brief nullable wrapper around object of type T. Used by async functions that return values - empty on function
	/// error/failure
	/// @tparam T
	template<typename T>
	using Optional = tl::optional<T>;

	using MutableBufferView = asio::mutable_buffer;
	using ConstBufferView = asio::const_buffer;

	template<typename Object>
	using StableStorage = std::shared_ptr<Object>;

	template<typename Object, class... _Types>
	StableStorage<Object> MakeStable(_Types&&... _Args)
	{
		return std::make_shared<Object>(std::forward<_Types>(_Args)...);
	}

	using Timestamp = std::uint64_t;
	using GalleryIndex = int;
} // namespace Modio

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
	#include "ghc/filesystem.hpp"
namespace Modio
{
	namespace filesystem = ghc::filesystem;
}
#endif
