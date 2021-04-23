#pragma once
#include "ModioGeneratedVariables.h"
#include "modio/core/ModioModCollectionEntry.h"
#include "modio/core/ModioStdTypes.h"
#include "modio/detail/userdata/ModioUserDataContainer.h"

namespace Modio
{
	namespace Detail
	{
		class SDKSessionData
		{
		public:
			/// <summary>
			/// Initializes the static SDK state with the specified GameID and APIKey. Fails if the SDK is
			/// already initialized/running whilst this is done.
			/// </summary>
			/// <param name="GameID"></param>
			/// <param name="APIKey"></param>
			/// <param name="Environment"></param>
			/// <returns> false if the SDK is already running and initialized</returns>
			MODIO_IMPL static bool Initialize(Modio::GameID GameID, const Modio::ApiKey& APIKey,
											  Modio::Environment Environment);

			MODIO_IMPL static void Deinitialize();

			MODIO_IMPL static bool IsInitialized();
			MODIO_IMPL static Modio::GameID CurrentGameID();
			MODIO_IMPL static const Modio::ApiKey& CurrentAPIKey();
			MODIO_IMPL static Modio::Environment GetEnvironment();

			MODIO_IMPL static bool IsModManagementEnabled();
			MODIO_IMPL static void AllowModManagement();
			MODIO_IMPL static void DisableModManagement();

			MODIO_IMPL static void MarkAsRateLimited();
			MODIO_IMPL static bool IsRateLimited();
			MODIO_IMPL static ModCollection& GetSystemModCollection();

			MODIO_IMPL static ModCollection FilterSystemModCollectionByUserSubscriptions();
			MODIO_IMPL static void InitializeForAuthenticatedUser(Modio::Detail::AuthenticatedUser AuthenticatedUser,
													   Modio::Detail::OAuthToken AuthToken);
			MODIO_IMPL static const Modio::Optional<Modio::Detail::OAuthToken> GetAuthenticationToken();

			MODIO_IMPL static Modio::UserSubscriptionList& GetUserSubscriptions();

			MODIO_IMPL static Modio::Detail::Buffer SerializeUserData();

			MODIO_IMPL static bool DeserializeUserDataFromBuffer(Modio::Detail::Buffer UserDataBuffer);

			MODIO_IMPL static void ClearUserData();

			MODIO_IMPL static const Modio::Optional<Modio::User> GetAuthenticatedUser();

			MODIO_IMPL static const Modio::Optional<Modio::Detail::Avatar> GetAuthenticatedUserAvatar();
			MODIO_IMPL static void AddToDeferredUnsubscriptions(Modio::ModID Mod);
			MODIO_IMPL static bool HasDeferredUnsubscriptions();

			MODIO_IMPL static Modio::ModID GetDeferredUnsubscription();

			MODIO_IMPL static void RemoveDeferredUnsubscription(Modio::ModID NoLongerDeferredUnsubscription);

			MODIO_IMPL static void SetUserModManagementCallback(
				std::function<void(Modio::ModManagementEvent)> Callback);

			MODIO_IMPL static void FlushModManagementLog();

			MODIO_IMPL static ModEventLog& GetModManagementEventLog();

			/// <summary>
			/// Initializes a ModProgressInfo for the specified mod, storing it in global state. This method is only
			/// intended for use by InstallOrUpdateMod
			/// </summary>
			/// <param name="ID">ModID for the mod to begin reporting progress on</param>
			/// <returns>Weak pointer to the ModProgressInfo, or null if a mod is already downloading/updating
			/// </returns>
			MODIO_IMPL static std::weak_ptr<Modio::ModProgressInfo> StartModDownloadOrUpdate(Modio::ModID ID);

			MODIO_IMPL static bool CancelModDownloadOrUpdate(Modio::ModID ID);

			/// <summary>
			/// Resets the internal ModProgressInfo object such that calls to GetModProgress return an empty optional
			/// indicating no mods are currently updating or installing. This method is only intended for use by
			/// InstallOrUpdateMod
			/// </summary>
			MODIO_IMPL static void FinishModDownloadOrUpdate();

			/// <summary>
			/// Fetches a static snapshot of the progress of the current download or update.
			/// </summary>
			/// <returns> Static copy of the progress data for the current download/update, or empty if no such
			/// operation is being performed</returns>
			MODIO_IMPL static Modio::Optional<const Modio::ModProgressInfo> GetModProgress();

		private:
			MODIO_IMPL SDKSessionData(Modio::GameID GameID, const Modio::ApiKey& APIKey,
									  Modio::Environment Environment);
			MODIO_IMPL SDKSessionData();

			// This may not need to be public, can probably just expose static accessors that call it
			MODIO_IMPL static SDKSessionData& Get();

			Modio::GameID GameID;
			Modio::ApiKey APIKey;
			Modio::Environment Environment;
			bool bInitialized = false;
			bool bModManagementEnabled = false;
			bool bShutdownRequested = false;

			// Implemented as shared_ptr because that way operations that need to alter the state of the entry can get a
			// cheap reference to the original without the lack of safety from a potentially dangling raw reference
			std::shared_ptr<Modio::ModProgressInfo> CurrentModInProgress;
			std::function<void(Modio::ModManagementEvent)> ModManagementEventCallback;
			Modio::ModCollection SystemModCollection;
			Modio::Detail::UserDataContainer UserData;
			// We may need to make this a shared pointer and give a reference to operations so if we shut down they
			// write into the stale log instead
			Modio::ModEventLog EventLog;
			bool bRateLimited = false;
			std::chrono::system_clock::time_point RateLimitedStart;
		};
	} // namespace Detail
} // namespace Modio

#ifndef MODIO_SEPARATE_COMPILATION
#include "ModioSDKSessionData.ipp"
#endif