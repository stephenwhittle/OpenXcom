#pragma once

#include "modio/cache/ModioCacheService.h"
#include "modio/core/ModioBuffer.h"
#include "modio/core/ModioCoreTypes.h"
#include "modio/detail/CoreOps.h"
#include "modio/detail/ModioAuthenticatedUser.h"
#include "modio/detail/ModioSDKSessionData.h"
#include "modio/detail/schema/AccessTokenObject.h"
#include "modio/detail/CoreOps.h"
#include "modio/http/ModioHttpParams.h"
#include "modio/userdata/ModioUserDataService.h"

#include "modio/detail/AsioWrapper.h"

#include <asio/yield.hpp>
namespace Modio
{
	namespace Detail
	{
		class AuthenticateUserByEmailOp
		{
		public:
			AuthenticateUserByEmailOp(Modio::EmailAuthCode EmailCode) : EmailCode(EmailCode) {}

			template<typename OperationState>
			void operator()(OperationState& Self, Modio::ErrorCode ec = {})
			{
				using namespace Modio;

				Detail::UserDataService& UserDataService =
					Detail::Services::GetGlobalService<Detail::UserDataService>();

				reenter(CoroutineState)
				{
					yield Detail::ComposedOps::PerformRequestAndGetResponseAsync(
						Local.ResponseBodyBuffer,
						ExchangeEmailSecurityCodeRequest.AppendPayloadValue(
							Modio::Detail::Constants::APIStrings::SecurityCode, EmailCode.InternalCode),
						Detail::CachedResponse::Disallow, std::move(Self));

					if (ec)
					{
						Self.complete(ec);
						return;
					}

					Local.AuthResponse =
						Detail::MarshalResponse<Detail::Schema::AccessTokenObject>(Local.ResponseBodyBuffer);
					Local.ResponseBodyBuffer.Clear();

					yield Detail::ComposedOps::PerformRequestAndGetResponseAsync(
						Local.ResponseBodyBuffer,
						Modio::Detail::GetAuthenticatedUserRequest.SetAuthTokenOverride(Local.AuthResponse.AccessToken),
						Detail::CachedResponse::Disallow, std::move(Self));

					if (ec)
					{
						Self.complete(ec);
						return;
					}

					Local.NewlyAuthenticatedUser =
						Detail::MarshalResponse<Modio::Detail::AuthenticatedUser>(Local.ResponseBodyBuffer);
					Local.ResponseBodyBuffer.Clear();

					Modio::Detail::SDKSessionData::InitializeForAuthenticatedUser(
						std::move(Local.NewlyAuthenticatedUser), Modio::Detail::OAuthToken(Local.AuthResponse));

					Detail::Services::GetGlobalService<Detail::CacheService>().ClearCache();
					yield UserDataService.SaveUserDataToStorageAsync(std::move(Self));

					Self.complete({});
					return;
				}
			}

		private:
			asio::coroutine CoroutineState;
			Modio::EmailAuthCode EmailCode;
			struct
			{
				Modio::Detail::DynamicBuffer ResponseBodyBuffer;
				Modio::Detail::Schema::AccessTokenObject AuthResponse;
				Modio::Detail::AuthenticatedUser NewlyAuthenticatedUser;
			} Local;
		};
	} // namespace Detail
} // namespace Modio
#include <asio/unyield.hpp>
