#pragma once
// Implementation header - do not include directly

#ifdef MODIO_SEPARATE_COMPILATION
	#include "modio/ModioSDK.h"
#endif

#include "modio/core/ModioCoreTypes.h"
#include "modio/detail/ops/AuthenticateUserByEmail.h"
#include "modio/detail/ops/RequestEmailAuthCode.h"
#include "modio/detail/ops/user/GetUserMedia.h"
#include "modio/detail/ops/auth/AuthenticateUserByDiscord.h"
#include "modio/detail/ops/auth/AuthenticateUserBySteam.h"
#include "modio/detail/ops/auth/AuthenticateUserBySwitchID.h"
#include "modio/detail/ops/auth/AuthenticateUserByGog.h"
#include "modio/detail/ops/auth/AuthenticateUserByXBoxLive.h"
#include "modio/detail/ops/auth/AuthenticateUserByItch.h"
#include "modio/impl/SDKPreconditionChecks.ipp"
#include "modio/userdata/ModioUserDataService.h"

namespace Modio
{
	void RequestEmailAuthCodeAsync(Modio::EmailAddress EmailAddress, std::function<void(Modio::ErrorCode)> Callback)
	{
		return asio::async_compose<std::function<void(Modio::ErrorCode)>, void(Modio::ErrorCode)>(
			Modio::Detail::RequestEmailAuthCode(std::move(EmailAddress.InternalAddress)), Callback,
			Modio::Detail::Services::GetGlobalContext().get_executor());
	}

	void AuthenticateUserExternalAsync(Modio::AuthenticationParams User, Modio::AuthenticationProvider Provider,
									   std::function<void(Modio::ErrorCode)> Callback)
	{
		if (Modio::Detail::RequireSDKIsInitialized(Callback) && Modio::Detail::RequireNotRateLimited(Callback) && Modio::Detail::RequireUserIsNOTAuthenticated(Callback))
		{
			switch (Provider)
			{
				case AuthenticationProvider::Epic:
					// AuthenticateUserWithEpicAsync(User, Callback);
					break;
				case AuthenticationProvider::GoG:
					Modio::Detail::AuthenticateUserByGoGAsync(User,Callback);
					break;
				case AuthenticationProvider::Itch:
					Modio::Detail::AuthenticateUserByItchAsync(User,Callback);
					break;
				case AuthenticationProvider::Steam:
					Modio::Detail::AuthenticateUserBySteamAsync(User,Callback);
					break;
				case AuthenticationProvider::XboxLive:
					Modio::Detail::AuthenticateUserByXBoxLiveAsync(User,Callback);
					break;
				case AuthenticationProvider::Switch:
					Modio::Detail::AuthenticateUserBySwitchIDAsync(User, Callback);
					break;
				case AuthenticationProvider::Discord:
					Modio::Detail::AuthenticateUserByDiscordAsync(User, Callback);
					break;
			}
		}
	}

	void AuthenticateUserEmailAsync(Modio::EmailAuthCode AuthenticationCode,
									std::function<void(Modio::ErrorCode)> Callback)
	{
		if (Modio::Detail::RequireUserIsNOTAuthenticated(Callback))
		{
			return asio::async_compose<std::function<void(Modio::ErrorCode)>, void(Modio::ErrorCode)>(
				Modio::Detail::AuthenticateUserByEmail(AuthenticationCode), Callback,
				Modio::Detail::Services::GetGlobalContext().get_executor());
		}
	}

	void ClearUserDataAsync(std::function<void(Modio::ErrorCode)> Callback)
	{
		if (Modio::Detail::RequireSDKIsInitialized(Callback) && Modio::Detail::RequireNotRateLimited(Callback) &&
			Modio::Detail::RequireUserIsAuthenticated(Callback))
		{
			return Modio::Detail::Services::GetGlobalService<Modio::Detail::UserDataService>().async_ClearUserData(
				Callback);
		}
	}

	Modio::Optional<Modio::User> QueryUserProfile()
	{
		return Modio::Detail::SDKSessionData::GetAuthenticatedUser();
	}

	void GetUserMediaAsync(Modio::AvatarSize AvatarSize,
						   std::function<void(Modio::ErrorCode, Modio::Optional<Modio::filesystem::path>)> Callback)
	{
		if (Modio::Detail::RequireSDKIsInitialized(Callback) && Modio::Detail::RequireNotRateLimited(Callback) &&
			Modio::Detail::RequireUserIsAuthenticated(Callback))
		{
			return asio::async_compose<std::function<void(Modio::ErrorCode, Modio::Optional<Modio::filesystem::path>)>,
									   void(Modio::ErrorCode, Modio::Optional<Modio::filesystem::path>)>(
				Modio::Detail::GetUserMedia(Modio::Detail::SDKSessionData::CurrentGameID(),
											Modio::Detail::SDKSessionData::CurrentAPIKey(), AvatarSize),
				Callback, Modio::Detail::Services::GetGlobalContext().get_executor());
		}
	}

} // namespace Modio
