#pragma once

#include "modio/core/ModioServices.h"
#include "modio/detail/ops/DownloadFile.h"
#include "modio/detail/ops/compression/ExtractAllToFolder.h"
#include "modio/detail/ops/http/PerformRequestAndGetResponse.h"
#include "modio/http/ModioHttpParams.h"
#include "modio/http/ModioHttpService.h"
#include <asio.hpp>

namespace Modio
{
	namespace Detail
	{
		namespace ComposedOps
		{
			/// <summary>
			///
			/// </summary>
			/// <typeparam name="CompletionTokenType"></typeparam>
			/// <param name="Response"></param>
			/// <param name="RequestParameters"></param>
			/// <param name="AllowCachedResponse">Ignored in the case of a request that's targetting a non-GET endpoint,
			/// and always disallow caching</param> <param name="Token"></param> <returns></returns>
			template<typename CompletionTokenType>
			auto async_PerformRequestAndGetResponse(Modio::Detail::DynamicBuffer Response,
													Modio::Detail::HttpRequestParams RequestParameters,
													Modio::Detail::CachedResponse AllowCachedResponse,
													CompletionTokenType&& Token)
			{
				return asio::async_compose<CompletionTokenType, void(Modio::ErrorCode)>(
					PerformRequestAndGetResponse(
						Response, RequestParameters,
						Modio::Detail::Services::GetGlobalService<Modio::Detail::HttpService>().GetAPIRequestTicket(),
						AllowCachedResponse),
					Token, Modio::Detail::Services::GetGlobalContext().get_executor());
			}

			template<typename CompletionTokenType>
			auto async_DownloadFile(Modio::Detail::HttpRequestParams DownloadParameters,
									Modio::filesystem::path DestinationPath,
									Modio::Optional<std::weak_ptr<Modio::ModProgressInfo>> ModProgress,
									CompletionTokenType&& Token)
			{
				return asio::async_compose<CompletionTokenType, void(Modio::ErrorCode)>(
					DownloadFile(
						DownloadParameters, DestinationPath,
						Modio::Detail::Services::GetGlobalService<Modio::Detail::HttpService>().GetFileDownloadTicket(),
						ModProgress),
					Token, Modio::Detail::Services::GetGlobalContext().get_executor());
			}

			template<typename CompletionTokenType>
			auto async_DownloadImage(Modio::Detail::HttpRequestParams DownloadParameters,
									 Modio::filesystem::path DestinationPath, CompletionTokenType&& Token)
			{
				return asio::async_compose<CompletionTokenType, void(Modio::ErrorCode)>(
					DownloadFile(
						DownloadParameters, DestinationPath,
						Modio::Detail::Services::GetGlobalService<Modio::Detail::HttpService>().GetAPIRequestTicket(),
						{}),
					Token, Modio::Detail::Services::GetGlobalContext().get_executor());
			}

			template<typename CompletionTokenType>
			auto async_ExtractAllFiles(Modio::filesystem::path ArchiveFile, Modio::filesystem::path DestinationPath,
									   Modio::Optional<std::weak_ptr<Modio::ModProgressInfo>> ProgressInfo,
									   CompletionTokenType&& Token)
			{
				return asio::async_compose<CompletionTokenType, void(Modio::ErrorCode, Modio::FileSize)>(
					ExtractAllToFolder(ArchiveFile, DestinationPath, ProgressInfo), Token,
					Modio::Detail::Services::GetGlobalContext().get_executor());
			}
		} // namespace ComposedOps
	} // namespace Detail
} // namespace Modio
