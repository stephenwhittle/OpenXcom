#pragma once
#include "modio/detail/ModioSDKSessionData.h"
#include "modio/detail/ops/modmanagement/ProcessNextModInUserCollection.h"
#include "modio/detail/ops/FetchExternalUpdates.h"

#include <asio.hpp>
#include <asio/yield.hpp>
#include <memory>
namespace Modio
{
	namespace Detail
	{
		/// <summary>
		/// Internal operation that processes the next mod entry in the current collection that requires some kind of
		/// action (installation, update, uninstallation)
		/// </summary>
		class ModManagementLoop
		{
			std::unique_ptr<asio::steady_timer> IdleTimer = nullptr;
			asio::coroutine CoroutineState;
			std::uint8_t ExternalUpdateCounter = 0;

		public:
			template<typename CoroType>
			void operator()(CoroType& Self, Modio::ErrorCode ec = {})
			{

				if (ec == Modio::GenericError::OperationCanceled)
				{
					Self.complete(ec);
					return;
				}

				reenter(CoroutineState)
				{
					while (Modio::Detail::SDKSessionData::IsModManagementEnabled())
					{
						if (ExternalUpdateCounter == 0)
						{
							//yield Modio::Detail::FetchExternalUpdatesAsync(std::move(Self));
						}

						//todo: @modio-core ensure that ops like FetchExternalUpdates translate their error codes into public codes for errors like Not Authorized
						if (ec) {
							Self.complete(ec);
							return;
						}
						
						// This operation gets the user subscriptions, filters the system mod list based on those, then
						// processes the next mod in that filtered list that requires some kind of management operation
						// (installation, update, etc).
						// It flags the mod with any error state that it encounters, so we don't need to handle that
						// here, just sleep and try to process the next mod
						yield Modio::Detail::async_ProcessNextModInUserCollection(std::move(Self));

						// lazy-initialize the timer
						if (IdleTimer == nullptr)
						{
							IdleTimer = std::make_unique<asio::steady_timer>(
								Modio::Detail::Services::GetGlobalContext().get_executor());
						}

						// Sleep for one second
						IdleTimer->expires_after(std::chrono::seconds(1));
						yield IdleTimer->async_wait(std::move(Self));
						
						ExternalUpdateCounter++;
						ExternalUpdateCounter = ExternalUpdateCounter % 15;
					}
					Self.complete({});
				}
			}
		};

		template<typename ManagementLoopEndCallback>
		auto async_BeginModManagementLoop(ManagementLoopEndCallback&& OnLoopEnded)
		{
			return asio::async_compose<ManagementLoopEndCallback, void(Modio::ErrorCode)>(
				ModManagementLoop(), OnLoopEnded, Modio::Detail::Services::GetGlobalContext().get_executor());
		}
	} // namespace Detail
} // namespace Modio
#include <asio/unyield.hpp>