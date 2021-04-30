#pragma once

#include "modio/core/ModioCoreTypes.h"
#include "modio/core/entities/ModioModDetails.h"
#include "modio/detail/CoreOps.h"
#include <asio/coroutine.hpp>

#include <asio/yield.hpp>

namespace Modio
{
	namespace Detail
	{
		class GetModDetails
		{
			Modio::Detail::DynamicBuffer ResponseBodyBuffer;
			Modio::GameID GameID;
			Modio::ApiKey ApiKey;
			Modio::ModID ModId;

			asio::coroutine CoroutineState;

		public:
			GetModDetails(Modio::GameID GameID, Modio::ApiKey ApiKey, Modio::ModID ModId)
				: GameID(GameID),
				  ApiKey(ApiKey),
				  ModId(ModId)
			{
			}

			template<typename CoroType>
			void operator()(CoroType& Self, Modio::ErrorCode ec = {})
			{
				reenter(CoroutineState)
				{
					yield Modio::Detail::ComposedOps::async_PerformRequestAndGetResponse(
						ResponseBodyBuffer,
						Modio::Detail::GetModfilesRequest.SetGameID(GameID).SetModID(ModId),
						Modio::Detail::CachedResponse::Disallow, std::move(Self));

					if (ec)
					{
						Self.complete(ec, {});
						return;
					}

					Self.complete(ec, MarshalResponse<Modio::ModDetails>(ResponseBodyBuffer));
					return;
				}
			}
		};
	} // namespace Detail
} // namespace Modio