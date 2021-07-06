#pragma once

#include "modio/core/ModioCoreTypes.h"
#include "modio/detail/entities/ModioLogo.h"
#include "modio/detail/ops/ModioLogoImageType.h"
#include "modio/detail/ops/ModioDownloadImage.h"
#include <asio/coroutine.hpp>
#include <asio/yield.hpp>

namespace Modio
{
	namespace Detail
	{
		class GetModMediaLogoOp
		{
			asio::coroutine CoroState;

			// Parameters
			Modio::GameID GameID;
			Modio::ApiKey ApiKey;
			Modio::ModID ModId;
			Modio::LogoSize LogoSize;

			// State needed to persist during coroutine
			struct
			{
				Modio::Detail::DynamicBuffer ResponseBodyBuffer;
				Modio::StableStorage<Modio::filesystem::path> DestinationPath;
				Modio::Detail::Logo Logo;
			} OpState;

		public:
			GetModMediaLogoOp(Modio::GameID GameID, Modio::ApiKey ApiKey, Modio::ModID ModId, Modio::LogoSize LogoSize)
				: GameID(GameID),
				  ApiKey(ApiKey),
				  ModId(ModId),
				  LogoSize(LogoSize)
			{
				OpState.DestinationPath = Modio::MakeStable<Modio::filesystem::path>();
			}

			template<typename CoroType>
			void operator()(CoroType& Self, Modio::ErrorCode ec = {})
			{
				reenter(CoroState)
				{
					// Fetch the details about the request from the server. Let's hope it's in the cache (would be
					// nice if we could extend the cache for this call)
					yield Modio::Detail::ComposedOps::PerformRequestAndGetResponseAsync(
						OpState.ResponseBodyBuffer, Modio::Detail::GetModRequest.SetGameID(GameID).SetModID(ModId),
						Modio::Detail::CachedResponse::Allow, std::move(Self));

					if (ec)
					{
						// FAILED
						Self.complete(ec, {});
						return;
					}

					// Marshall the result of the request
					OpState.Logo = Modio::Detail::MarshalSubobjectResponse<Modio::Detail::Logo>(
						"logo", OpState.ResponseBodyBuffer);

					yield Modio::Detail::DownloadImageAsync(LogoImageType(ModId, LogoSize, OpState.Logo),
															OpState.DestinationPath, std::move(Self));

					if (ec)
					{
						Self.complete(ec, {});
						return;
					}

					Self.complete({}, std::move(*OpState.DestinationPath));
				}
			}
		};
	} // namespace Detail
} // namespace Modio

#include <asio/unyield.hpp>
