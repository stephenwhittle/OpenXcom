#pragma once
#include "ModioGeneratedVariables.h"
#include "modio/core/ModioModCollectionEntry.h"
#include "modio/core/ModioStdTypes.h"
#include "modio/detail/ModioAuthenticatedUser.h"
#include "modio/detail/userdata/ModioUserProfile.h"
#include <vector>

namespace Modio
{
	namespace Detail
	{
		struct UserDataContainer
		{
		public:
			MODIO_IMPL void ResetUserData();

			MODIO_IMPL void InitializeForAuthenticatedUser(Modio::Detail::AuthenticatedUser AuthenticatedUser,
														   Modio::Detail::OAuthToken AuthToken);

			// @todo: Making copy of user object
			MODIO_IMPL const Modio::Optional<Modio::User> GetAuthenticatedUser() const;

			// @todo: Making copy of avatar object
			MODIO_IMPL const Modio::Optional<Modio::Detail::Avatar> GetAuthenticatedUserAvatar() const;

			MODIO_IMPL const Modio::Optional<Modio::Detail::OAuthToken> GetAuthenticationToken() const;

			MODIO_IMPL const bool IsValid() const;

			Modio::UserSubscriptionList UserSubscriptions;
			std::vector<Modio::ModID> DeferredUnsubscriptions;
			Modio::Optional<Modio::Detail::ProfileData> AuthenticatedProfile;

			Modio::Optional<Modio::filesystem::path> LastUsedModDirectory;

			friend void to_json(nlohmann::json& Json, const Modio::Detail::UserDataContainer& UserData)
			{
				Json = nlohmann::json::object(
					{UserData.UserSubscriptions,
					 {Modio::Detail::Constants::JSONKeys::DeferredUnsubscribes, UserData.DeferredUnsubscriptions}});
				if (UserData.AuthenticatedProfile)
				{
					Json[Modio::Detail::Constants::JSONKeys::OAuth] = UserData.AuthenticatedProfile->GetToken();
					Json[Modio::Detail::Constants::JSONKeys::UserProfile] = UserData.AuthenticatedProfile->GetUser();
					Json[Modio::Detail::Constants::JSONKeys::Avatar] = UserData.AuthenticatedProfile->GetAvatar();
				}
				Json[Modio::Detail::Constants::JSONKeys::LastUsedModDirectory] = UserData.LastUsedModDirectory.value().u8string();
			}
			friend void from_json(const nlohmann::json& Json, Modio::Detail::UserDataContainer& UserData)
			{
				Modio::Detail::AuthenticatedUser AuthenticatedUser;
				bool ParseStatus = Modio::Detail::ParseSafe(Json, AuthenticatedUser.User,
															Modio::Detail::Constants::JSONKeys::UserProfile);
				ParseStatus &= Modio::Detail::ParseSafe(Json, AuthenticatedUser.Avatar,
														Modio::Detail::Constants::JSONKeys::Avatar);
				Modio::Detail::OAuthToken Token;
				ParseStatus &= Modio::Detail::ParseSafe(Json, Token, Modio::Detail::Constants::JSONKeys::OAuth);
				if (ParseStatus)
				{
					UserData.InitializeForAuthenticatedUser(AuthenticatedUser, Token);
				}

				Modio::Detail::ParseSafe(Json, UserData.UserSubscriptions,
										 Modio::Detail::Constants::JSONKeys::UserSubscriptionList);
				Modio::Detail::ParseSafe(Json, UserData.DeferredUnsubscriptions,
										 Modio::Detail::Constants::JSONKeys::DeferredUnsubscribes);
				std::string TmpPath;
				Modio::Detail::ParseSafe(Json, TmpPath, Modio::Detail::Constants::JSONKeys::LastUsedModDirectory);
				if (!TmpPath.empty())
				{
					UserData.LastUsedModDirectory = Modio::filesystem::path(TmpPath);
				}
			}
		};
	} // namespace Detail
} // namespace Modio

#ifndef MODIO_SEPARATE_COMPILATION
	#include "ModioUserDataContainer.ipp"
#endif
