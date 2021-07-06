#ifdef MODIO_SEPARATE_COMPILATION
	#include "modio/detail/ModioSDKSessionData.h"
#endif

namespace Modio

{
	namespace Detail
	{
		SDKSessionData& SDKSessionData::Get()
		{
			static SDKSessionData Instance;
			return Instance;
		}

		bool SDKSessionData::Initialize(Modio::GameID GameID, const Modio::ApiKey& APIKey,
										Modio::Environment Environment)
		{
			// If we are already initializing or done with init, bail
			if (Get().CurrentInitializationState != InitializationState::NotInitialized)
			{
				return false;
			}
			else
			{
				Get() = std::move(SDKSessionData(GameID, APIKey, Environment));
				Get().CurrentInitializationState = InitializationState::Initializing;
				return true;
			}
		}

		void SDKSessionData::Deinitialize()
		{
			Get().FlushModManagementLog();

			Get().CurrentInitializationState = InitializationState::NotInitialized;
		}

		void SDKSessionData::ConfirmInitialize()
		{
			Get().CurrentInitializationState = InitializationState::InitializationComplete;
		}

		bool SDKSessionData::IsInitialized()
		{
			return Get().CurrentInitializationState == InitializationState::InitializationComplete;
		}

		Modio::GameID SDKSessionData::CurrentGameID()
		{
			return Get().GameID;
		}

		const Modio::ApiKey& SDKSessionData::CurrentAPIKey()
		{
			return Get().APIKey;
		}

		Modio::Environment SDKSessionData::GetEnvironment()
		{
			return Get().Environment;
		}

		bool SDKSessionData::IsModManagementEnabled()
		{
			return Get().bModManagementEnabled;
		}

		void SDKSessionData::AllowModManagement()
		{
			Get().bModManagementEnabled = true;
		}

		void SDKSessionData::DisableModManagement()
		{
			Get().bModManagementEnabled = false;
		}

		void SDKSessionData::MarkAsRateLimited()
		{
			Get().bRateLimited = true;
			Get().RateLimitedStart = std::chrono::system_clock::now();
		}

		bool SDKSessionData::IsRateLimited()
		{
			// TODO: @modio-core need to compare the timestamp of start to now and work out time elapsed, reset flag if
			// need be
			return Get().bRateLimited;
		}

		Modio::ModCollection& SDKSessionData::GetSystemModCollection()
		{
			return Get().SystemModCollection;
		}

		Modio::ModCollection SDKSessionData::FilterSystemModCollectionByUserSubscriptions()
		{
			return Get().SystemModCollection.FilterByUserSubscriptions(Get().UserData.UserSubscriptions);
		}

		void SDKSessionData::InitializeForAuthenticatedUser(Modio::Detail::AuthenticatedUser AuthenticatedUser,
															Modio::Detail::OAuthToken AuthToken)
		{
			Get().UserData.InitializeForAuthenticatedUser(std::move(AuthenticatedUser), std::move(AuthToken));
		}

		const Modio::Optional<Modio::Detail::OAuthToken> SDKSessionData::GetAuthenticationToken()
		{
			return Get().UserData.GetAuthenticationToken();
		}

		Modio::UserSubscriptionList& SDKSessionData::GetUserSubscriptions()
		{
			return Get().UserData.UserSubscriptions;
		}

		Modio::Detail::Buffer SDKSessionData::SerializeUserData()
		{
			nlohmann::json Data(Get().UserData);
			std::string UserData = Data.dump();
			Modio::Detail::Buffer DataBuffer(UserData.size());
			std::copy(UserData.begin(), UserData.end(), DataBuffer.begin());
			return DataBuffer;
		}

		bool SDKSessionData::DeserializeUserDataFromBuffer(Modio::Detail::Buffer UserDataBuffer)
		{
			nlohmann::json UserDataJson = Modio::Detail::ToJson(std::move(UserDataBuffer));
			from_json(UserDataJson, Get().UserData);
			return Get().UserData.IsValid();
		}

		void SDKSessionData::ClearUserData()
		{
			Get().UserData.ResetUserData();
		}

		const Modio::Optional<Modio::User> SDKSessionData::GetAuthenticatedUser()
		{
			return Get().UserData.GetAuthenticatedUser();
		}

		const Modio::Optional<Modio::Detail::Avatar> SDKSessionData::GetAuthenticatedUserAvatar()
		{
			return Get().UserData.GetAuthenticatedUserAvatar();
		}

		void SDKSessionData::AddToDeferredUnsubscriptions(Modio::ModID Mod)
		{
			Get().UserData.DeferredUnsubscriptions.push_back(Mod);
		}

		bool SDKSessionData::HasDeferredUnsubscriptions()
		{
			return Get().UserData.DeferredUnsubscriptions.size() > 0;
		}

		Modio::ModID SDKSessionData::GetDeferredUnsubscription()
		{
			return Get().UserData.DeferredUnsubscriptions.front();
		}

		void SDKSessionData::RemoveDeferredUnsubscription(Modio::ModID NoLongerDeferredUnsubscription)
		{
			UserDataContainer& UserData = Get().UserData;
			UserData.DeferredUnsubscriptions.erase(std::remove(UserData.DeferredUnsubscriptions.begin(),
															   UserData.DeferredUnsubscriptions.end(),
															   NoLongerDeferredUnsubscription),
												   UserData.DeferredUnsubscriptions.end());
		}

		Modio::Optional<Modio::filesystem::path> SDKSessionData::GetLastUsedModDirectory()
		{
			return Get().UserData.LastUsedModDirectory;
		}

		bool SDKSessionData::SetDefaultModDirectory(Modio::filesystem::path DefaultDirectory)
		{
			UserDataContainer& UserData = Get().UserData;
			if (!UserData.LastUsedModDirectory.has_value())
			{
				UserData.LastUsedModDirectory = DefaultDirectory;
				return true;
			}
			return false;
		}

		void SDKSessionData::SetUserModManagementCallback(std::function<void(Modio::ModManagementEvent)> Callback)
		{
			Get().ModManagementEventCallback = Callback;
		}

		void SDKSessionData::FlushModManagementLog()
		{
			if (auto&& Callback = Get().ModManagementEventCallback)
			{
				for (auto&& Event : Get().EventLog.ClearLog())
				{
					Callback(Event);
				}
			}
		}

		Modio::ModEventLog& SDKSessionData::GetModManagementEventLog()
		{
			return Get().EventLog;
		}

		std::weak_ptr<Modio::ModProgressInfo> SDKSessionData::StartModDownloadOrUpdate(Modio::ModID ID)
		{
			if (Get().CurrentModInProgress != nullptr)
			{
				return std::weak_ptr<Modio::ModProgressInfo>();
			}
			else
			{
				Get().CurrentModInProgress = std::make_shared<Modio::ModProgressInfo>(ID);
				return Get().CurrentModInProgress;
			}
		}

		bool SDKSessionData::CancelModDownloadOrUpdate(Modio::ModID ID)
		{
			if (Get().CurrentModInProgress != nullptr)
			{
				if (Get().CurrentModInProgress->ID == ID)
				{
					FinishModDownloadOrUpdate();
					return true;
				}
			}
			return false;
		}

		void SDKSessionData::FinishModDownloadOrUpdate()
		{
			Get().CurrentModInProgress.reset();
		}

		Modio::Optional<const Modio::ModProgressInfo> SDKSessionData::GetModProgress()
		{
			if (Get().CurrentModInProgress == nullptr)
			{
				return {};
			}
			else
			{
				return *(Get().CurrentModInProgress);
			}
		}

		SDKSessionData::SDKSessionData() {}

		SDKSessionData::SDKSessionData(Modio::GameID GameID, const Modio::ApiKey& APIKey,
									   Modio::Environment Environment)
			: GameID(GameID),
			  APIKey(APIKey),
			  Environment(Environment),
			  CurrentInitializationState(InitializationState::NotInitialized)
		{}
	} // namespace Detail

} // namespace Modio