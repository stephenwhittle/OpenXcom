#pragma once

#include "modio/core/ModioBuffer.h"
#include "modio/core/ModioStdTypes.h"
#include "modio/detail/ModioOperationQueue.h"
#include "modio/file/ModioFile.h"
#include "modio/http/ModioHttpRequest.h"
#include <asio.hpp>
#include "modio/detail/ModioObjectTrack.h"
#include "modio/core/ModioModCollectionEntry.h"
#include <asio/yield.hpp>

/// <summary>
/// Operation for downloading an arbitrary file to an arbitrary filesystem path
/// </summary>
class DownloadFile : public Modio::Detail::BaseOperation<DownloadFile>
{
	Modio::StableStorage<Modio::Detail::HttpRequest> Request;
	asio::coroutine Coroutine;
	Modio::Detail::DynamicBuffer ResponseBodyBuffer;
	Modio::StableStorage<Modio::Detail::File> File;

	Modio::StableStorage<std::uintmax_t> CurrentFilePosition;
	Modio::StableStorage<bool> EndOfFileReached;
	Modio::Optional<std::weak_ptr<Modio::ModProgressInfo>> ProgressInfo;
	bool bDownloadingMod = false;

	struct DownloadFileImpl 
	{
		Modio::Detail::OperationQueue::Ticket DownloadTicket;

	public:
		DownloadFileImpl(Modio::Detail::OperationQueue::Ticket DownloadTicket) : DownloadTicket(std::move(DownloadTicket)) {};
	};

	Modio::StableStorage<DownloadFileImpl> Impl;

public:
	
	DownloadFile(const Modio::Detail::HttpRequestParams RequestParams, Modio::filesystem::path DestinationPath,
				 Modio::Detail::OperationQueue::Ticket DownloadTicket,
				 Modio::Optional<std::weak_ptr<Modio::ModProgressInfo>> ProgressInfo)
		: ProgressInfo(ProgressInfo)
	{
		File = std::make_shared<Modio::Detail::File>(DestinationPath+=Modio::filesystem::path(".download"), false);
		CurrentFilePosition = std::make_shared<std::uintmax_t>(File->GetFileSize() - (File->GetFileSize() % (1024 * 1024)));
		File->Truncate(Modio::FileOffset(*CurrentFilePosition));
		File->Seek(Modio::FileOffset(*CurrentFilePosition));
		Request =
			std::make_shared<Modio::Detail::HttpRequest>(RequestParams.SetRange(Modio::FileOffset(*CurrentFilePosition), {}));
		EndOfFileReached = std::make_shared<bool>(false);
		Impl = std::make_shared<DownloadFileImpl>(std::move(DownloadTicket));

	};

	template<typename CoroType>
	void operator()(CoroType& Self, Modio::ErrorCode ec = {})
	{
		// May need to manually check bDownloadingMod and lock progressinfo if this returns true when we never had a
		// value in the pointer
		if (ProgressInfo.has_value() && ProgressInfo->expired())
		{
			// If we abort a download, truncate the file to zero bytes so that we start from the beginning at next download (can't delete it here as it's locked)
			if (File)
			{
				File->Truncate(Modio::FileOffset(0));
				//Modio::Detail::Services::GetGlobalService<Modio::Detail::FileService>().DeleteFile(File->GetPath());
			}
			Self.complete(Modio::make_error_code(Modio::ModManagementError::InstallOrUpdateCancelled));
			return;
		}
		
		// Temporary optional to hold buffers without causing issues with coroutine switch statement
		Modio::Optional<Modio::Detail::Buffer> CurrentBuffer;

		reenter(Coroutine)
		{
			yield Impl->DownloadTicket.async_WaitForTurn(std::move(Self));

			if (ec)
			{
				Self.complete(ec);
				return;
			}

			Modio::Detail::Logger().Log(Modio::LogLevel::Info, Modio::LogCategory::Http, "Beginning download of file {}",
								File->GetPath().u8string());
			
			yield Request->async_Send(std::move(Self));

			if (ec)
			{
				Self.complete(ec);
				return;
			}
			
			yield Request->async_ReadResponseHeaders(std::move(Self));

			if (ec)
			{
				Self.complete(ec);
				return;
			}

			if (Request->GetResponseCode() != 200 && Request->GetResponseCode() != 206)
			{				
				Self.complete(Modio::make_error_code(Modio::HttpError::ResourceNotAvailable));
				return;
			}
			
			while (!ec)
			{
				// Read in a chunk from the response
				yield Request->async_ReadSomeFromResponseBody(ResponseBodyBuffer, std::move(Self));
				if (ec && ec != Modio::make_error_code(Modio::GenericError::EndOfFile))
				{
					Self.complete(ec);
					return;
				}
				else if (ec == Modio::make_error_code(Modio::GenericError::EndOfFile))
				{
					// Cache the EOF state, because ec gets mutated by the calls to async_WriteSomeAt below
					*EndOfFileReached = true;
				}
				// ReadSomeFromResponseBody should only store a single chunk into our DynamicBuffer
				// But just in case, iterate through all the internal buffers and write them to file

				while ((CurrentBuffer = ResponseBodyBuffer.TakeInternalBuffer()))
				{
					// TODO: @Modio-core implement async_WriteSome?
					*CurrentFilePosition += CurrentBuffer->GetSize();
					
					if (ProgressInfo.has_value()) {
						if (!ProgressInfo->expired())
						{
							ProgressInfo->lock()->CurrentlyDownloadedBytes = Modio::FileSize(*CurrentFilePosition);
						}
						else
						{
							Self.complete(Modio::make_error_code(Modio::ModManagementError::InstallOrUpdateCancelled));
							return;
						}
					}
					
					yield File->async_WriteSomeAt(*CurrentFilePosition - CurrentBuffer->GetSize(),
												  std::move(CurrentBuffer.value()), std::move(Self));
				}
				
				if (*EndOfFileReached)
				{
					Modio::filesystem::path Destination = File->GetPath().replace_extension();
					if (!File->Rename(Destination)) {
						Modio::Detail::Logger().Log(Modio::LogLevel::Error, Modio::LogCategory::File,
										   "Could not rename downloaded file to {}", Destination.u8string());
						File.reset();

						Self.complete(Modio::make_error_code(Modio::FilesystemError::UnableToCreateFile));
						return;
					}
					else
					{
						Modio::Detail::Logger().Log(Modio::LogLevel::Info, Modio::LogCategory::Http, "Download of {} completed",
											File->GetPath().u8string());
						// Clean up
						File.reset();
						Self.complete(Modio::ErrorCode {});
						return;
					}
				}
			}

			if (ec && ec != Modio::make_error_code(Modio::GenericError::EndOfFile))
			{
				Self.complete(ec);
				return;
			}
			else
			{
				File.reset();
				Self.complete(Modio::ErrorCode {});
				return;
			}
		}
	}
};
#include <asio/unyield.hpp>