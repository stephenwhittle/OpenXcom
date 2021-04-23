#pragma once

#ifdef MODIO_SEPARATE_COMPILATION
	#include "modio/ModioSDK.h"
#endif

#include "modio/core/ModioModCollectionEntry.h"
#include "modio/core/ModioServices.h"
#include "modio/core/ModioStdTypes.h"
#include "modio/detail/ModioSDKSessionData.h"
#include "modio/detail/ops/ModManagementLoop.h"
#include "modio/detail/ops/SubscribeToMod.h"
#include "modio/detail/ops/UnsubscribeFromMod.h"
#include "modio/detail/ops/modmanagement/ForceUninstallMod.h"
#include "modio/impl/SDKPreconditionChecks.ipp"
#include "modio/userdata/ModioUserDataService.h"
#include <asio.hpp>
namespace Modio
{
	void EnableModManagement(std::function<void(Modio::ModManagementEvent)> Callback)
	{
		Modio::Detail::SDKSessionData::SetUserModManagementCallback(Callback);
		Modio::Detail::SDKSessionData::AllowModManagement();
		Modio::Detail::async_BeginModManagementLoop([](Modio::ErrorCode ec) mutable {
			if (ec)
			{
				Modio::Detail::Logger().Log(LogLevel::Info, Modio::LogCategory::Core,
											"Mod Management Loop halted: status message {}", ec.message());
			}
		});
	}
	void DisableModManagement()
	{
		Modio::Detail::SDKSessionData::SetUserModManagementCallback(std::function<void(Modio::ModManagementEvent)> {});
		Modio::Detail::SDKSessionData::DisableModManagement();
	}

	void FetchExternalUpdatesAsync(std::function<void(Modio::ErrorCode)> OnFetchDone)
	{
		if (Modio::Detail::RequireSDKIsInitialized(OnFetchDone) && Modio::Detail::RequireNotRateLimited(OnFetchDone) &&
			Modio::Detail::RequireUserIsAuthenticated(OnFetchDone) &&
			Modio::Detail::RequireModManagementEnabled(OnFetchDone))
		{
			Modio::Detail::FetchExternalUpdatesAsync(OnFetchDone);
		}
	}

	void SubscribeToModAsync(Modio::ModID ModToSubscribeTo, std::function<void(Modio::ErrorCode)> OnSubscribeComplete)
	{
		if (Modio::Detail::RequireSDKIsInitialized(OnSubscribeComplete) &&
			Modio::Detail::RequireNotRateLimited(OnSubscribeComplete) &&
			Modio::Detail::RequireUserIsAuthenticated(OnSubscribeComplete) &&
			Modio::Detail::RequireModManagementEnabled(OnSubscribeComplete))

		{
			return asio::async_compose<std::function<void(Modio::ErrorCode)>, void(Modio::ErrorCode)>(
				Modio::Detail::SubscribeToMod(Modio::Detail::SDKSessionData::CurrentGameID(),
											  Modio::Detail::SDKSessionData::CurrentAPIKey(), ModToSubscribeTo),
				OnSubscribeComplete, Modio::Detail::Services::GetGlobalContext().get_executor());
		}
	}

	void UnsubscribeFromModAsync(Modio::ModID ModToUnsubscribeFrom,
								 std::function<void(Modio::ErrorCode)> OnUnsubscribeComplete)
	{
		if (Modio::Detail::RequireSDKIsInitialized(OnUnsubscribeComplete) &&
			Modio::Detail::RequireNotRateLimited(OnUnsubscribeComplete) &&
			Modio::Detail::RequireUserIsAuthenticated(OnUnsubscribeComplete) &&
			Modio::Detail::RequireModManagementEnabled(OnUnsubscribeComplete))
		{
			return Modio::Detail::UnsubscribeFromModAsync(ModToUnsubscribeFrom, OnUnsubscribeComplete);
		}
	}

	bool IsModManagementBusy()
	{
		if (!Modio::Detail::SDKSessionData::IsModManagementEnabled())
		{
			return false;
		}

		Modio::Detail::UserDataService& UserService =
			Modio::Detail::Services::GetGlobalService<Modio::Detail::UserDataService>();
		Modio::ModCollection UserModCollection =
			Modio::Detail::SDKSessionData::FilterSystemModCollectionByUserSubscriptions();

		for (auto ModEntry : UserModCollection.Entries())
		{
			Modio::ModState CurrentState = ModEntry.second->GetModState();
			if (CurrentState != Modio::ModState::Installed)
			{
				return true;
			}
		}
		return false;
	}

	Modio::Optional<Modio::ModProgressInfo> QueryCurrentModUpdate()
	{
		return Modio::Detail::SDKSessionData::GetModProgress();
	}

	std::map<Modio::ModID, Modio::ModCollectionEntry> QueryUserSubscriptions()
	{
		Modio::Detail::UserDataService& UserService =
			Modio::Detail::Services::GetGlobalService<Modio::Detail::UserDataService>();
		Modio::ModCollection UserModCollection =
			Modio::Detail::SDKSessionData::FilterSystemModCollectionByUserSubscriptions();
		std::map<Modio::ModID, ModCollectionEntry> UserSubscriptions;

		// TODO: @modio-core double check that this copies ModEntry.second
		for (auto ModEntry : UserModCollection.Entries())
		{
			UserSubscriptions.emplace(std::make_pair(ModEntry.first, (*ModEntry.second)));
		}
		return UserSubscriptions;
	}

	std::map<Modio::ModID, Modio::ModCollectionEntry> QueryUserInstallations(bool bIncludeOutdatedMods)
	{
		Modio::Detail::UserDataService& UserService =
			Modio::Detail::Services::GetGlobalService<Modio::Detail::UserDataService>();
		Modio::ModCollection UserModCollection =
			Modio::Detail::SDKSessionData::FilterSystemModCollectionByUserSubscriptions();
		std::map<Modio::ModID, ModCollectionEntry> UserInstallations;

		// TODO: @modio-core double check that this copies ModEntry.second

		// Only return mods that are either installed, and if bIncludeOutdatedMods mods that are installed but have
		// an update available that isn't currently being processed
		for (auto ModEntry : UserModCollection.Entries())
		{
			if (ModEntry.second->GetModState() == ModState::Installed)
			{
				UserInstallations.emplace(std::make_pair(ModEntry.first, (*ModEntry.second)));
			}
			if (bIncludeOutdatedMods && ModEntry.second->GetModState() == ModState::UpdatePending)
			{
				UserInstallations.emplace(std::make_pair(ModEntry.first, (*ModEntry.second)));
			}
		}
		return UserInstallations;
	}

	std::map<Modio::ModID, Modio::ModCollectionEntry> QuerySystemInstallations()
	{
		std::map<Modio::ModID, ModCollectionEntry> InstalledMods;
		Modio::ModCollection& AllInstalledMods = Modio::Detail::SDKSessionData::GetSystemModCollection();
		for (auto& ModEntry : AllInstalledMods.Entries())
		{
			InstalledMods.emplace(std::make_pair(ModEntry.first, (*ModEntry.second)));
		}
		return InstalledMods;
	}

	void ForceUninstallModAsync(Modio::ModID ModToRemove, std::function<void(Modio::ErrorCode)> Callback) {
		if (Modio::Detail::RequireSDKIsInitialized(Callback) &&
			Modio::Detail::RequireUserIsAuthenticated(Callback) &&
			Modio::Detail::RequireModManagementEnabled(Callback) &&
			Modio::Detail::RequireUserNotSubscribed(ModToRemove,Callback))
		{
			asio::async_compose<std::function<void(Modio::ErrorCode)>, void(Modio::ErrorCode)>(
				Modio::Detail::ForceUninstallMod(ModToRemove), Callback, Modio::Detail::Services::GetGlobalContext().get_executor());
		}
	}

} // namespace Modio