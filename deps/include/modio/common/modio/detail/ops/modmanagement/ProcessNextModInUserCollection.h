#pragma once
#include "modio/core/ModioLogger.h"
#include "modio/core/ModioModCollectionEntry.h"
#include "modio/core/ModioServices.h"
#include "modio/detail/ModioSDKSessionData.h"
#include "modio/detail/ops/modmanagement/InstallOrUpdateMod.h"
#include "modio/detail/ops/modmanagement/UninstallMod.h"
#include "modio/detail/ops/SaveModCollectionToStorage.h"
#include "modio/userdata/ModioUserDataService.h"
#include <asio.hpp>
#include <asio/yield.hpp>
namespace Modio
{
	namespace Detail
	{
		/// <summary>
		/// Internal operation. Searches the user's mod collection for the next mod marked as requiring
		/// installation,update, or uninstallation, then initiates that operation
		/// </summary>
		class ProcessNextModInUserCollection
		{
		public:
			template<typename CoroType>
			void operator()(CoroType& Self, Modio::ErrorCode ec = {})
			{
				reenter(CoroutineState)
				{

					{
						Modio::Detail::UserDataService& UserService =
							Modio::Detail::Services::GetGlobalService<Modio::Detail::UserDataService>();
						EntryToProcess = nullptr;
						// Check for pending uninstallations regardless of user
						for (auto ModEntry : Modio::Detail::SDKSessionData::GetSystemModCollection().Entries())
						{
							if (ModEntry.second->GetModState() == Modio::ModState::UninstallPending)
							{
								EntryToProcess = ModEntry.second;
							}
						}

						// If no pending uninstallations, check for this users installs or updates
						if (!EntryToProcess)
						{
							Modio::ModCollection UserModCollection = Modio::Detail::SDKSessionData::FilterSystemModCollectionByUserSubscriptions();
							for (auto ModEntry : UserModCollection.Entries())
							{
								Modio::ModState CurrentState = ModEntry.second->GetModState();
								if (CurrentState == Modio::ModState::InstallationPending ||
									CurrentState == Modio::ModState::UpdatePending)
								{
									if (ModEntry.second->ShouldRetryInstallation())
									{
										EntryToProcess = ModEntry.second;
									}
								}
							}
						}
					}
					if (EntryToProcess == nullptr)
					{
						Self.complete({});
						return;
					}
					if (EntryToProcess->GetModState() == Modio::ModState::InstallationPending ||
						EntryToProcess->GetModState() == Modio::ModState::UpdatePending)
					{
						// Does this need to be a separate operation or could we provide a parameter to specify
						// we only want to update if it's already installed or something?
						yield Modio::Detail::async_InstallOrUpdateMod(EntryToProcess->GetID(), std::move(Self));
						Modio::Detail::SDKSessionData::GetModManagementEventLog().AddEntry(
								Modio::ModManagementEvent {EntryToProcess->GetID(), 
								EntryToProcess->GetModState() == Modio::ModState::InstallationPending ? Modio::ModManagementEvent::EventType::Installed : Modio::ModManagementEvent::EventType::Updated,
								ec
								});
						if (ec)
						{
							if (ec.category() != Modio::HttpErrorCategory() && ec != Modio::ModManagementError::InstallOrUpdateCancelled)
							{
								EntryToProcess->MarkModNoRetry();
							}
							
							Self.complete(ec);
							return;
						}
						else
						{
							yield Modio::Detail::SaveModCollectionToStorageAsync(std::move(Self));

							Self.complete({});
							return;
						}
					}
					else if (EntryToProcess->GetModState() == Modio::ModState::UninstallPending)
					{
						yield Modio::Detail::async_UninstallMod(EntryToProcess->GetID(), std::move(Self));
						Modio::Detail::SDKSessionData::GetModManagementEventLog().AddEntry(Modio::ModManagementEvent {
							EntryToProcess->GetID(),
							Modio::ModManagementEvent::EventType::Uninstalled,
							ec});
						if (ec)
						{
							Self.complete(ec);
							return;
						}
						else
						{
							yield Modio::Detail::SaveModCollectionToStorageAsync(std::move(Self));

							Self.complete({});
							return;
						}
					}
				}
			}

		private:
			asio::coroutine CoroutineState;
			std::shared_ptr<Modio::ModCollectionEntry> EntryToProcess;
		};

		template<typename ProcessNextCallback>
		auto async_ProcessNextModInUserCollection(ProcessNextCallback&& OnProcessComplete)
		{
			return asio::async_compose<ProcessNextCallback, void(Modio::ErrorCode)>(
				ProcessNextModInUserCollection(), OnProcessComplete,
				Modio::Detail::Services::GetGlobalContext().get_executor());
		}
	} // namespace Detail
} // namespace Modio

#include <asio/unyield.hpp>