// MIRRORED TO gdk/file/FileObjectImplementation.h, UPDATE THAT FILE IF THIS IS UPDATED
#pragma once
#include "modio/core/ModioLogger.h"
#include "modio/core/ModioStdTypes.h"
#include "modio/detail/file/IFileObjectImplementation.h"
#include <asio.hpp>
// TODO: @modio-xdk decide if we want to wrap fileapi in nominmax etc
#include <atomic>
#include <chrono>
#include <fileapi.h>
namespace Modio
{
	namespace Detail
	{
		class FileObjectImplementation : public Modio::Detail::IFileObjectImplementation
		{
			filesystem::path FilePath;
			filesystem::path BasePath;
			HANDLE FileHandle;
			// Strand so that IO ops don't get performed simultaneously
			asio::strand<asio::io_context::executor_type>* Strand;
			std::atomic<bool> OperationInProgress = false;
			std::atomic<std::int32_t> NumWaiters = 0;
			asio::steady_timer OperationQueue;
			Modio::FileOffset CurrentSeekOffset = Modio::FileOffset(0);
			bool CancelRequested = false;

		public:
			FileObjectImplementation(asio::io_context& ParentContext, filesystem::path BasePath)
				: FileHandle(INVALID_HANDLE_VALUE),
				  BasePath(BasePath),
				  FilePath(),
				  Strand(nullptr),
				  OperationQueue(ParentContext, std::chrono::steady_clock::time_point::max()),
				  CurrentSeekOffset(0)
			{}

			~FileObjectImplementation()
			{
				Destroy();
			}
			void Destroy()
			{
				if (FileHandle != INVALID_HANDLE_VALUE)
				{
					CloseHandle(FileHandle);
					FileHandle = INVALID_HANDLE_VALUE;
				}
			}
			void Close()
			{
				Destroy();
			}

			void CancelAll()
			{
				CancelRequested = true;
			}

			bool ShouldCancel()
			{
				return CancelRequested;
			}

			template<typename OperationType>
			void BeginExclusiveOperation(OperationType&& Operation)
			{
				if (OperationInProgress.exchange(true))
				{
					++NumWaiters;
					// todo: @Modio-XDK post this to file strand
					OperationQueue.async_wait(
						asio::bind_executor(Modio::Detail::Services::GetGlobalContext().get_executor(),
											[Op = std::move(Operation)](asio::error_code ec) mutable { Op(); }));
				}
				else
				{
					asio::post(Modio::Detail::Services::GetGlobalContext().get_executor(), std::move(Operation));
				}
			}

			void FinishExclusiveOperation()
			{
				OperationInProgress.exchange(false);

				if (NumWaiters > 0)
				{
					--NumWaiters;
					OperationQueue.cancel_one();
				}
			}

			filesystem::path GetPath()
			{
				return FilePath;
			}

			virtual bool Rename(Modio::filesystem::path NewPath) override
			{
				std::error_code ec;
				if (FileHandle != INVALID_HANDLE_VALUE)
				{
					CloseHandle(FileHandle);
				}
				Modio::filesystem::rename(FilePath, NewPath, ec);
				if (ec)
				{
					return false;
				}
				FilePath = NewPath;

				ec = OpenFile(FilePath, false);
				return !ec;
			}

			virtual void Truncate(Modio::FileOffset Offset) override
			{
				LARGE_INTEGER Length;
				Length.QuadPart = Offset;
				SetFilePointerEx(FileHandle, Length, NULL, FILE_BEGIN);
				SetEndOfFile(FileHandle);
			}

			virtual std::size_t GetSize() override
			{
				if (FileHandle != INVALID_HANDLE_VALUE)
				{
					LARGE_INTEGER FileSize;
					GetFileSizeEx(FileHandle, &FileSize);
					return FileSize.QuadPart;
				}
				return INVALID_FILE_SIZE;
			}

			virtual void Seek(Modio::FileOffset Offset, Modio::Detail::SeekDirection Direction) override
			{
				switch (Direction)
				{
					case Modio::Detail::SeekDirection::Absolute:
						CurrentSeekOffset = Offset;
						break;
					case Modio::Detail::SeekDirection::Forward:
						CurrentSeekOffset += Offset;
						break;
					case Modio::Detail::SeekDirection::Backward:
						if (CurrentSeekOffset < Offset)
						{
							CurrentSeekOffset = FileOffset(0);
						}
						else
						{
							CurrentSeekOffset -= Offset;
						}
				}
			}

			Modio::FileOffset Tell()
			{
				return CurrentSeekOffset;
			}

			void SetFileStrand(asio::strand<asio::io_context::executor_type>& FileStrand)
			{
				Strand = &FileStrand;
			}

			asio::strand<asio::io_context::executor_type>& GetFileStrand()
			{
				return *Strand;
			}

			std::error_code CreateFile(filesystem::path FilePath)
			{
				return OpenFile(FilePath, true);
			}

			std::error_code OpenFile(filesystem::path FilePath, bool bOverwrite = false)
			{
				Modio::ErrorCode ec;
				filesystem::create_directories(FilePath.parent_path(), ec);
				if (ec)
				{
					return ec;
				}

				this->FilePath = FilePath;
				FileHandle = ::CreateFileW(this->FilePath.generic_wstring().c_str(), GENERIC_READ | GENERIC_WRITE,
										  FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
										  bOverwrite ? CREATE_ALWAYS : OPEN_ALWAYS, FILE_FLAG_OVERLAPPED, NULL);
				if (FileHandle != INVALID_HANDLE_VALUE)
				{
					return std::error_code {};
				}
				else
				{
					DWORD Error = GetLastError();
					// post failure
					return std::error_code(Error, std::system_category());
				}
			}

			HANDLE GetFileHandle()
			{
				return FileHandle;
			}
		};
	} // namespace Detail
} // namespace Modio
