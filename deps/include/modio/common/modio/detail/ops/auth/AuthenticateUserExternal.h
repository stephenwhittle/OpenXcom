#pragma once
#include "modio/http/ModioHttpParams.h"
#include "modio/userdata/ModioUserDataService.h"
#include <memory>
#include <asio.hpp>
namespace Modio
{
	namespace Detail
	{
#include <asio/yield.hpp>
		class AuthenticateUserExternal
		{
		public:
			AuthenticateUserExternal(Modio::Detail::HttpRequestParams AuthenticationParams)
			{
				LocalState = std::make_shared<Impl>();
				LocalState->AuthenticationParams = AuthenticationParams;
			};
			template<typename CoroType>
			void operator()(CoroType& Self, Modio::ErrorCode ec = {})
			{
				auto& UserDataService = Modio::Detail::Services::GetGlobalService<Modio::Detail::UserDataService>();
				reenter(LocalState->CoroutineState)
				{
					yield Modio::Detail::ComposedOps::async_PerformRequestAndGetResponse(
						LocalState->ResponseBuffer, LocalState->AuthenticationParams, Detail::CachedResponse::Disallow, std::move(Self));
					if (ec)
					{
						Self.complete(ec);
						return;
					}

					LocalState->AuthResponse =
						Detail::MarshalResponse<Detail::Schema::AccessTokenObject>(LocalState->ResponseBuffer);
					LocalState->ResponseBuffer.Clear();

					yield Modio::Detail::ComposedOps::async_PerformRequestAndGetResponse(
						LocalState->ResponseBuffer,
						Modio::Detail::GetAuthenticatedUserRequest.SetAuthTokenOverride(LocalState->AuthResponse.AccessToken),
						Modio::Detail::CachedResponse::Disallow, std::move(Self));

					if (ec)
					{
						Self.complete(ec);
						return;
					}

					LocalState->AuthenticatedUserData = Modio::Detail::MarshalResponse<Modio::Detail::AuthenticatedUser>(LocalState->ResponseBuffer);
					LocalState->ResponseBuffer.Clear();

					if (!Modio::Detail::SDKSessionData::GetAuthenticatedUser() ||
												   LocalState->AuthenticatedUserData.User.UserId !=
													   Modio::Detail::SDKSessionData::GetAuthenticatedUser()->UserId)
					{
						yield UserDataService.async_ClearUserData(std::move(Self));
						Modio::Detail::Services::GetGlobalService<Modio::Detail::CacheService>().ClearCache();
					}
					
					Modio::Detail::SDKSessionData::InitializeForAuthenticatedUser(
						std::move(LocalState->AuthenticatedUserData), Modio::Detail::OAuthToken(LocalState->AuthResponse));
					yield UserDataService.SaveUserDataToStorageAsync(std::move(Self));
					Self.complete({});
					return;
				}
			}

		private:
			struct Impl
			{
				asio::coroutine CoroutineState;
				Modio::Detail::DynamicBuffer ResponseBuffer;
				Modio::Detail::HttpRequestParams AuthenticationParams;
				Modio::Detail::Schema::AccessTokenObject AuthResponse;
				Modio::Detail::AuthenticatedUser AuthenticatedUserData;
			};
			Modio::StableStorage<Impl> LocalState;
		};
#include <asio/unyield.hpp>
	} // namespace Detail
} // namespace Modio