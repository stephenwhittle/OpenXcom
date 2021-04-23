#pragma once

#include "modio/core/ModioBuffer.h"
#include "modio/core/entities/ModioModTagOptions.h"
#include "modio/detail/CoreOps.h"
#include "modio/detail/ModioJsonHelpers.h"
#include "modio/detail/ModioSDKSessionData.h"
#include "modio/http/ModioHttpParams.h"
#include <asio.hpp>

#include <asio/yield.hpp>
namespace Modio
{
	namespace Detail
	{
		class GetModTags
		{
		public:
			template<typename CoroType>
			void operator()(CoroType& Self, Modio::ErrorCode ec = {})
			{
				reenter(CoroutineState)
				{
					yield Modio::Detail::ComposedOps::async_PerformRequestAndGetResponse(
						TagResponseBuffer,
						Modio::Detail::GetGameTagOptionsRequest.SetGameID(Modio::Detail::SDKSessionData::CurrentGameID()),
						Modio::Detail::CachedResponse::Allow, std::move(Self));
					if (ec)
					{
						Self.complete(ec, {});
						return;
					}
					Modio::ModTagOptions Options = MarshalResponse<Modio::ModTagOptions>(TagResponseBuffer);
					// need to convert this to our own tag object here then
					Self.complete({}, Options);
				}
			}

		private:
			asio::coroutine CoroutineState;
			Modio::Detail::DynamicBuffer TagResponseBuffer;
		};
	} // namespace Detail
} // namespace Modio
#include <asio/unyield.hpp>