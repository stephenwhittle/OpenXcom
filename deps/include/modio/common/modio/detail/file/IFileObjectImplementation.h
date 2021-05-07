#pragma once
#include "modio/core/ModioStdTypes.h"

#include<asio.hpp>

namespace Modio
{
	namespace Detail
	{
		enum class SeekDirection
		{
			Absolute,
			Forward,
			Backward
		};
		/// @brief Interface class defining operations that a platform file object needs to support
		class IFileObjectImplementation
		{
		public:
			virtual ~IFileObjectImplementation() {};

			virtual void SetFileStrand(asio::strand<asio::io_context::executor_type>& FileStrand) = 0;

			virtual asio::strand<asio::io_context::executor_type>& GetFileStrand() = 0;

			virtual std::error_code CreateFile(filesystem::path FilePath) = 0;

			virtual std::error_code OpenFile(filesystem::path FilePath, bool bOverwrite = false) = 0;

			/// @brief OS-specific file size calculation
			/// @return Size of the underlying file
			/// @todo {core} Should this return std::uintmax_t instead?
			virtual std::size_t GetSize() = 0;
			/// @brief Retrieves file path
			/// @return Path to the underlying file
			virtual Modio::filesystem::path GetPath() = 0;
			/// @brief Renames a file to a new name
			/// @param NewPath The new name for the file
			/// @return true on success
			virtual bool Rename(Modio::filesystem::path NewPath) = 0;
			/// @brief Sets the file length to the desired value
			/// @param Offset new end length for the file
			/// @todo {core} should this truncate the seek pointer?
			virtual void Truncate(Modio::FileOffset Offset) = 0;
			/// @brief Sets the offset for future streamed reads or writes to the file
			/// @param Offset The position to read or write from
			virtual void Seek(Modio::FileOffset Offset, Modio::Detail::SeekDirection Direction) = 0;

			virtual void Destroy() = 0;
		};
	} // namespace Detail
} // namespace Modio