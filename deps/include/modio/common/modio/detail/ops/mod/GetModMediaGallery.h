#pragma once

#include "modio/core/ModioBuffer.h"
#include "modio/core/ModioStdTypes.h"
#include "modio/detail/ModioAuthenticatedUser.h"
#include "modio/detail/ops/ModioDownloadGalleryImage.h"
#include <asio/yield.hpp>

namespace Modio
{
	namespace Detail
	{
		class GetModMediaGallery
		{
		public:
			GetModMediaGallery(Modio::GameID GameID, Modio::ApiKey ApiKey, Modio::ModID ModId,
							   Modio::GallerySize GallerySize, Modio::GalleryIndex ImageIndex)
				: GameID(GameID),
				  ApiKey(ApiKey),
				  ModId(ModId),
				  GallerySize(GallerySize),
				  ImageIndex(ImageIndex)
			{
				OpState.DestinationPath = Modio::MakeStable<Modio::filesystem::path>();
			}

			template<typename CoroType>
			void operator()(CoroType& Self, Modio::ErrorCode ec = {})
			{
				reenter(CoroutineState)
				{
					if (ImageIndex < 0)
					{
						Self.complete(Modio::make_error_code(Modio::GenericError::IndexOutOfRange), {});
						return;
					}

					// Fetch the details about the request from the server. Let's hope it's in the cache (would be nice
					// if we could extend the cache for this call)
					yield Modio::Detail::ComposedOps::async_PerformRequestAndGetResponse(
						OpState.ResponseBodyBuffer, Modio::Detail::GetModRequest.SetGameID(GameID).SetModID(ModId),
						Modio::Detail::CachedResponse::Allow, std::move(Self));

					if (ec)
					{
						// FAILED
						Self.complete(ec, {});
						return;
					}

					OpState.GalleryList = Modio::Detail::MarshalSubobjectResponse<Modio::Detail::GalleryList>(
						"media", OpState.ResponseBodyBuffer);
					if (ImageIndex >= OpState.GalleryList.Size())
					{
						// FAILED
						Self.complete(Modio::make_error_code(Modio::GenericError::IndexOutOfRange), {});
						return;
					}
					
					yield Modio::Detail::DownloadImageAsync(
						GalleryImageType(ModId, OpState.GalleryList, GallerySize, ImageIndex), OpState.DestinationPath,
						std::move(Self));
					if (ec)
					{
						Self.complete(ec, {});
						return;
					}

					Self.complete({}, std::move(*OpState.DestinationPath));
				}
			}

		private:
			asio::coroutine CoroutineState;

			// Parameters
			Modio::GameID GameID;
			Modio::ApiKey ApiKey;
			Modio::ModID ModId;
			Modio::GallerySize GallerySize;
			Modio::GalleryIndex ImageIndex;

			// State that might get mutated during the coroutine
			struct
			{
				Modio::Detail::DynamicBuffer ResponseBodyBuffer;
				Modio::StableStorage<Modio::filesystem::path> DestinationPath;
				Modio::Detail::GalleryList GalleryList;
			} OpState;
		};
	} // namespace Detail
} // namespace Modio

#include <asio/unyield.hpp>
