#pragma once

#include "modio/core/ModioBuffer.h"
#include "modio/core/ModioStdTypes.h"
#include "modio/detail/ModioAuthenticatedUser.h"
#include "modio/detail/ops/ModioDownloadAvatar.h"
#include "modio/detail/ops/ModioDownloadImage.h"
#include <asio/yield.hpp>

namespace Modio
{
	namespace Detail
	{
		class GetModMediaAvatar
		{
		public:
			GetModMediaAvatar(Modio::GameID GameID, Modio::ApiKey ApiKey, Modio::ModID ModId,
							  Modio::AvatarSize AvatarSize)
				: GameID(GameID),
				  ApiKey(ApiKey),
				  ModId(ModId),
				  AvatarSize(AvatarSize)
			{
				OpState.DestinationPath = Modio::MakeStable<Modio::filesystem::path>();
			}

			template<typename CoroType>
			void operator()(CoroType& Self, Modio::ErrorCode ec = {})
			{
				reenter(CoroutineState)
				{
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

					// Marshall the result of the request
					OpState.User = Modio::Detail::MarshalSubobjectResponse<Modio::Detail::AuthenticatedUser>(
						"submitted_by", OpState.ResponseBodyBuffer);

					yield Modio::Detail::DownloadImageAsync(
						AvatarImageType(OpState.User.User.UserId, AvatarSize,
									  OpState.User.Avatar),
						OpState.DestinationPath, std::move(Self));

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
			Modio::AvatarSize AvatarSize;

			// State that might get mutated during the coroutine
			struct
			{
				Modio::Detail::DynamicBuffer ResponseBodyBuffer;
				Modio::StableStorage<Modio::filesystem::path> DestinationPath;
				Modio::Detail::AuthenticatedUser User;
			} OpState;
		};
	} // namespace Detail
} // namespace Modio

#include <asio/unyield.hpp>
