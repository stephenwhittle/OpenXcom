#pragma once

#include "modio/core/ModioStdTypes.h"
#include "modio/core/entities/ModioModInfo.h"
#include "modio/detail/CoreOps.h"
#include <asio.hpp>

#include <asio/yield.hpp>
namespace Modio
{
	namespace Detail
	{
		class GetModInfo
		{
			Modio::Detail::DynamicBuffer ResponseBodyBuffer;
			Modio::GameID GameID;
			Modio::ApiKey ApiKey;
			Modio::ModID ModId;

			asio::coroutine CoroutineState;
		public:
			GetModInfo(Modio::GameID GameID, Modio::ApiKey ApiKey, Modio::ModID ModId)
				: GameID(GameID),
				ApiKey(ApiKey),
				ModId(ModId)
			{
			}

			template<typename CoroType>
			void operator()(CoroType& Self, Modio::ErrorCode ec = {})
			{
				reenter (CoroutineState)
				{
					yield Modio::Detail::ComposedOps::async_PerformRequestAndGetResponse(
						ResponseBodyBuffer,
						Modio::Detail::GetModRequest.SetGameID(GameID).SetModID(ModId),
						Modio::Detail::CachedResponse::Allow, std::move(Self));

					if (ec)
					{
						Self.complete(ec, {});
						return;
					}

					Self.complete(ec, MarshalResponse<Modio::ModInfo>(ResponseBodyBuffer));
					return;
				}
			}
		};
	}
}
#include <asio/unyield.hpp>