#pragma once

#include "modio/core/ModioStdTypes.h"
#include "modio/detail/CoreOps.h"
#include "modio/core/entities/ModioModInfoList.h"
#include "modio/core/ModioBuffer.h"
#include "modio/core/ModioFilterParams.h"
#include <asio/coroutine.hpp>


#include <asio/yield.hpp>
namespace Modio
{
	namespace Detail
	{
		/// <summary>
		/// Calls the server and marshals the result into consumer structs
		/// </summary>
		class ListAllMods
		{
			Modio::Detail::DynamicBuffer ResponseBodyBuffer;
			Modio::FilterParams Filter;
			Modio::GameID GameID;

			asio::coroutine CoroutineState;
		public:
			ListAllMods(Modio::GameID GameID, FilterParams InFilter) :
				Filter(std::move(InFilter)),
				GameID(GameID)
			{
			}				

			template<typename CoroType>
			void operator()(CoroType& Self, Modio::ErrorCode ec = {})
			{
				reenter(CoroutineState)
				{					
					yield Modio::Detail::ComposedOps::async_PerformRequestAndGetResponse(
						ResponseBodyBuffer, Modio::Detail::GetModsRequest.SetGameID(GameID).SetFilterString(Filter.ToString()),
						Modio::Detail::CachedResponse::Allow, std::move(Self));
					if (ec)
					{
						Self.complete(ec, {});
						return;
					}

					Self.complete(ec, MarshalResponse<Modio::ModInfoList>(ResponseBodyBuffer));
					return;
				}
			}
		};
	}
}