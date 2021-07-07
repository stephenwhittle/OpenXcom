#pragma once

#include "modio/core/ModioBuffer.h"
#include "modio/core/ModioCoreTypes.h"
#include "modio/core/ModioFilterParams.h"
#include "modio/core/entities/ModioModInfoList.h"
#include "modio/detail/CoreOps.h"
#include <asio/coroutine.hpp>

#include <asio/yield.hpp>
namespace Modio
{
	namespace Detail
	{
		/// <summary>
		/// Calls the server and marshals the result into consumer structs
		/// </summary>
		class ListAllModsOp
		{
			Modio::Detail::DynamicBuffer ResponseBodyBuffer;
			Modio::FilterParams Filter;
			Modio::GameID GameID;

			asio::coroutine CoroutineState;

		public:
			ListAllModsOp(Modio::GameID GameID, FilterParams InFilter) : Filter(std::move(InFilter)), GameID(GameID) {}

			template<typename CoroType>
			void operator()(CoroType& Self, Modio::ErrorCode ec = {})
			{
				reenter(CoroutineState)
				{
					yield Modio::Detail::ComposedOps::PerformRequestAndGetResponseAsync(
						ResponseBodyBuffer,
						Modio::Detail::GetModsRequest.SetGameID(GameID).SetFilterString(Filter.ToString()),
						Modio::Detail::CachedResponse::Allow, std::move(Self));
					if (ec)
					{
						// Marshal all raw HTTP errors into generic RequestError
						// if (Modio::ErrorCodeMatches(ec, Modio::ErrorConditionTypes::RawHttpError)
						//{
						//	Self.complete(Modio::make_error_code(Modio::HttpError::InvalidResponse), {});
						//	return;
						//}
						Self.complete(ec, {});
						return;
					}

					{
						//Got the response OK, try to marshal to the expected type
						Modio::Optional<Modio::ModInfoList> List =
							TryMarshalResponse<Modio::ModInfoList>(ResponseBodyBuffer);
						// Marshalled OK
						if (List.has_value())
						{
							Self.complete(ec, std::move(List));
						}
						else
						{
							//Marshalling failed
							Self.complete(Modio::make_error_code(Modio::HttpError::InvalidResponse), {});
						}
						return;
					}
				}
			}
		};
	} // namespace Detail
} // namespace Modio