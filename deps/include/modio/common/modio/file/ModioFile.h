#pragma once
#include "modio/core/ModioBuffer.h"
#include "modio/core/ModioFilesystem.h"
#include "modio/core/ModioServices.h"
#include "modio/file/ModioFileService.h"
#include <asio.hpp>

namespace Modio
{
	namespace Detail
	{
		class File : public asio::basic_io_object<Modio::Detail::FileService>
		{
			Modio::filesystem::path FilePath;
			asio::strand<asio::io_context::executor_type> FileStrand;

		public:
			explicit File(Modio::filesystem::path FilePath, bool bOverwriteExisting = false)
				: asio::basic_io_object<Modio::Detail::FileService>(Modio::Detail::Services::GetGlobalContext()),
				  FilePath(FilePath),
				  FileStrand(asio::make_strand(Modio::Detail::Services::GetGlobalContext()))
			{
				get_implementation()->SetFileStrand(FileStrand);
				std::error_code ec = get_implementation()->OpenFile(FilePath, bOverwriteExisting);
				if (ec)
				{
					Modio::Detail::Logger().Log(Modio::LogLevel::Error, Modio::LogCategory::File,
												"Error code {} while trying to open {}", ec.value(), FilePath.string());
				}
			}

			explicit File(asio::io_context& Context, Modio::filesystem::path FilePath, bool bOverWriteExisting = false)
				: asio::basic_io_object<Modio::Detail::FileService>(Context),
				  FilePath(FilePath),
				  FileStrand(asio::make_strand(Context))
			{
				get_implementation()->SetFileStrand(FileStrand);
				std::error_code ec = get_implementation()->OpenFile(FilePath, bOverWriteExisting);
				if (ec)
				{
					Modio::Detail::Logger().Log(Modio::LogLevel::Error, Modio::LogCategory::File,
												"Error code {} while trying to open {}", ec.value(), FilePath.string());
				}
			};

			File(Modio::Detail::File&& Other)
				: asio::basic_io_object<Modio::Detail::FileService>(std::move(Other)),
				  FilePath(std::move(Other.FilePath)),
				  FileStrand(std::move(Other.FileStrand)) {};

			std::size_t GetFileSize()
			{
				return get_implementation()->GetSize();
			}

			filesystem::path GetPath()
			{
				return get_implementation()->GetPath();
			}

			bool Rename(Modio::filesystem::path NewPath)
			{
				return get_implementation()->Rename(NewPath);
			}

			void Truncate(Modio::FileOffset Offset)
			{
				get_implementation()->Truncate(Offset);
			}
			void Seek(Modio::FileOffset Offset,
					  Modio::Detail::SeekDirection Direction = Modio::Detail::SeekDirection::Absolute)
			{
				get_implementation()->Seek(Offset, Direction);
			}

			template<typename CompletionTokenType>
			auto async_WriteSomeAt(std::uintmax_t Offset, Modio::Detail::Buffer Buffer, CompletionTokenType&& Token)
			{
				return get_service().async_write_some_at(get_implementation(), Offset, std::move(Buffer),
														 std::forward<CompletionTokenType>(std::move(Token)));
			}

			template<typename CompletionTokenType>
			auto async_ReadSomeAt(std::uintmax_t Offset, std::uintmax_t Length, CompletionTokenType&& Token)
			{
				return get_service().async_read_some_at(get_implementation(), Offset, Length,
														std::forward<CompletionTokenType>(std::move(Token)));
			}

			template<typename CompletionTokenType>
			auto async_ReadSomeAt(std::uintmax_t Offset, std::uintmax_t MaxBytesToRead,
								  Modio::Detail::DynamicBuffer Destination, CompletionTokenType&& Token)
			{
				return get_service().async_read_some_at(get_implementation(), Offset, MaxBytesToRead, Destination,
														std::forward<CompletionTokenType>(std::move(Token)));
			}

			template<typename CompletionTokenType>
			auto async_Read(std::uintmax_t MaxBytesToRead, Modio::Detail::DynamicBuffer Destination,
							CompletionTokenType&& Token)
			{
				return get_service().async_read(get_implementation(), MaxBytesToRead, Destination,
												std::forward<CompletionTokenType>(std::move(Token)));
			}

			template<typename CompletionTokenType>
			auto async_Write(Modio::Detail::Buffer Buffer, CompletionTokenType&& Token)
			{
				return get_service().async_write(get_implementation(), std::move(Buffer),
												 std::forward<CompletionTokenType>(std::move(Token)));
			}
		};
	} // namespace Detail
} // namespace Modio
