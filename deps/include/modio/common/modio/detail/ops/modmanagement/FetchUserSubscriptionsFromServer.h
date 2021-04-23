#pragma once
#include "modio/core/ModioBuffer.h"
#include "modio/core/entities/ModioModInfoList.h"
#include "modio/core/entities/ModioPagedResult.h"
#include "modio/detail/CoreOps.h"
#include "modio/detail/ModioJsonHelpers.h"
#include <asio.hpp>
#include <asio/yield.hpp>
#include <fmt/format.h>
namespace Modio
{
	namespace Detail
	{
		class FetchUserSubscriptionsFromServer
		{
		public:
			template<typename CoroType>
			void operator()(CoroType& Self, Modio::ErrorCode ec = {})
			{
				reenter(CoroutineState)
				{
					do
					{
						// Because we're making a raw request here, manually add the filter to paginate 100 results at a
						// time We're going to gather all the results together at the end of this anyways so the biggest
						// pages give the best results because it means fewer REST calls
						yield Modio::Detail::ComposedOps::async_PerformRequestAndGetResponse(
							SubscriptionBuffer,
							Modio::Detail::GetUserSubscriptionsRequest.SetFilterString(
								fmt::format("_limit=100&_offset={}&game_id={}", CurrentResultIndex, Modio::Detail::SDKSessionData::CurrentGameID())),
							Modio::Detail::CachedResponse::Allow, std::move(Self));
						if (ec)
						{
							Self.complete(ec, {});
							return;
						}
						if (CollatedResults == nullptr)
						{
							CollatedResults = std::make_unique<Modio::ModInfoList>();
						}
						// append the results to a modinfolist in stable storage
						Modio::ModInfoList CurrentModInfoPage = MarshalResponse<Modio::ModInfoList>(SubscriptionBuffer);
						PageInfo = MarshalResponse<Modio::PagedResult>(SubscriptionBuffer);
						CollatedResults->Append(CurrentModInfoPage);
						CurrentResultIndex += 100;
					} while (CurrentResultIndex < PageInfo.GetTotalResultCount());
					
					Self.complete({}, *(CollatedResults.release()));
				}
			}

		private:
			asio::coroutine CoroutineState;
			Modio::Detail::DynamicBuffer SubscriptionBuffer;
			Modio::PagedResult PageInfo;
			std::unique_ptr<Modio::ModInfoList> CollatedResults;
			std::int32_t CurrentResultIndex = 0;
		};

		template<typename FetchCompleteCallback>
		auto async_FetchUserSubscriptionsFromServer(FetchCompleteCallback&& OnFetchComplete)
		{
			return asio::async_compose<FetchCompleteCallback, void(Modio::ErrorCode, Modio::Optional<Modio::ModInfoList>)>(
				FetchUserSubscriptionsFromServer(), OnFetchComplete,
				Modio::Detail::Services::GetGlobalContext().get_executor());
		}
	} // namespace Detail
} // namespace Modio
#include <asio/unyield.hpp>