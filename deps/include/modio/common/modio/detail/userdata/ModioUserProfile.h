#pragma once

#include "modio/core/ModioStdTypes.h"
#include "modio/core/entities/ModioModInfoList.h"
#include "modio/detail/ModioAuthenticatedUser.h"
#include "modio/detail/schema/AccessTokenObject.h"

#include <memory>

namespace Modio::Detail
{
	enum class OAuthTokenState : std::uint8_t
	{
		Valid,
		Expired,
		Invalid
	};

	struct OAuthToken
	{
		OAuthToken() = default;
		OAuthToken(const std::string& InToken, Modio::Timestamp InExpireDate)
			: Token(InToken),
			  ExpireDate(InExpireDate),
			  State(OAuthTokenState::Valid)
		{}
		OAuthToken(Modio::Detail::Schema::AccessTokenObject AccessToken)
			: OAuthToken(AccessToken.AccessToken, AccessToken.DateExpires)
		{}
		OAuthTokenState GetTokenState() const
		{
			auto Now = std::chrono::system_clock::now();
			std::size_t CurrentUTCTime =
				std::chrono::duration_cast<std::chrono::seconds>(Now.time_since_epoch()).count();

			if (State == OAuthTokenState::Valid)
			{
				return CurrentUTCTime <= ExpireDate ? OAuthTokenState::Valid : OAuthTokenState::Invalid;
			}
			return OAuthTokenState::Invalid;
		}
		operator bool() const
		{
			return GetTokenState() == OAuthTokenState::Valid;
		}
		const Modio::Optional<std::string> GetToken() const
		{
			if (GetTokenState() == OAuthTokenState::Valid)
			{
				return Token.value();
			}
			return {};
		}

		static inline Modio::Optional<std::string> NoToken {};
		friend void from_json(const nlohmann::json& Json, Modio::Detail::OAuthToken& Token)
		{
			Detail::ParseSafe(Json, Token.ExpireDate, "expiry");
			Detail::ParseSafe(Json, Token.State, "status");
			std::string TokenString = "";

			if (Detail::ParseSafe(Json, TokenString, "token"))
			{
				Token.Token = TokenString;
			}
			else
			{
				Token.State = OAuthTokenState::Invalid;
			}
		}
		friend void to_json(nlohmann::json& Json, const Modio::Detail::OAuthToken& Token)
		{
			if (Token.State == OAuthTokenState::Valid && Token.Token.has_value())
			{
				Json = nlohmann::json {{"expiry", Token.ExpireDate},
									   {"status", Token.State},
									   {"token", Token.Token.value()}};
			}
			else
			{
				Json = nlohmann::json {{"expiry", Token.ExpireDate}, {"status", OAuthTokenState::Invalid}};
			}
		}

	private:
		// Optional here so that the accessors can return references to avoid memcpy, will always be set
		Modio::Optional<std::string> Token;
		Modio::Timestamp ExpireDate;

	private:
		OAuthTokenState State = OAuthTokenState::Invalid;
	};

	struct ProfileData
	{
		ProfileData(Modio::Detail::AuthenticatedUser InUser, Modio::Detail::OAuthToken AccessToken)
			: User(InUser),
			  Token(AccessToken)
		{}

		const Modio::User& GetUser() const
		{
			return User.User;
		}

		const Modio::Detail::Avatar GetAvatar() const
		{
			return User.Avatar;
		}

		const Modio::Detail::OAuthToken& GetToken() const
		{
			return Token;
		}

	private:
		Modio::Detail::AuthenticatedUser User;
		Modio::Detail::OAuthToken Token;
	};
} // namespace Modio::Detail
