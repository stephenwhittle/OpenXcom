#pragma once
#include "modio/core/entities/ModioUser.h"
#include "modio/core/ModioModCollectionEntry.h"
#include "modio/detail/ops/userdata/SaveUserDataToStorage.h"
#include "modio/detail/ops/userdata/InitializeUserData.h"

#include <asio.hpp>
#include <vector>

#include <asio/yield.hpp>

namespace Modio
{
	
	namespace Detail
	{
		class UserDataService : public asio::detail::service_base<UserDataService>
		{
		public:
			explicit UserDataService(asio::io_context& IOService)
				: asio::detail::service_base<UserDataService>(IOService)
			{
				
			}

			void Shutdown()
			{
				
			}

			template<typename CompletionHandlerType>
			auto async_Initialize( CompletionHandlerType&& Handler)
			{
				return asio::async_compose<CompletionHandlerType, void(Modio::ErrorCode)>(
					InitializeUserData(), Handler, Modio::Detail::Services::GetGlobalContext().get_executor());
			}

			template<typename CompletionHandlerType>
			auto async_ClearUserData(CompletionHandlerType&& Handler)
			{
				ModCollection FilteredModCollection =
					Modio::Detail::SDKSessionData::FilterSystemModCollectionByUserSubscriptions();
				// This may require additional testing to ensure that we don't decrement the count further than we ought
				for (auto& Entry : FilteredModCollection.Entries())
				{
					Entry.second->RemoveLocalUserSubscription(Modio::Detail::SDKSessionData::GetAuthenticatedUser());
				}

				Modio::Detail::SDKSessionData::ClearUserData();
				return SaveUserDataToStorageAsync(std::forward<CompletionHandlerType>(Handler));			
			}

			template<typename CompletionHandlerType>
			auto SaveUserDataToStorageAsync(CompletionHandlerType&& Handler)
			{
				return asio::async_compose<CompletionHandlerType, void(Modio::ErrorCode)>(
					SaveUserDataToStorage(), Handler,
					Modio::Detail::Services::GetGlobalContext().get_executor());
			}

		private:
		};
	} // namespace Detail
} // namespace Modio
#include <asio/unyield.hpp>