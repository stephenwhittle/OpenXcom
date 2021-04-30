#pragma once
#include "modio/cache/ModioCacheService.h"
#include "modio/core/ModioServices.h"
#include "modio/detail/ModioSDKSessionData.h"
#include "modio/detail/ops/LoadModCollectionFromStorage.h"
#include "modio/file/ModioFileService.h"
#include "modio/http/ModioHttpService.h"
#include "modio/userdata/ModioUserDataService.h"

#include <asio.hpp>
#include <asio/yield.hpp>

class ServiceInitialization
{
	asio::coroutine CoroutineState;
	Modio::GameID GameID;
	Modio::ApiKey APIKey;
	Modio::Environment Environment;
	Modio::UserHandleType User;

public:
	ServiceInitialization(Modio::GameID GameID, Modio::ApiKey APIKey, Modio::Environment Environment,
						  Modio::UserHandleType User)
		: GameID(GameID),
		  APIKey(APIKey),
		  Environment(Environment),
		  User(User)
	{}

	template<typename CoroType>
	void operator()(CoroType& self, std::error_code ec = {})
	{
		reenter(CoroutineState)
		{
			if (!Modio::Detail::SDKSessionData::Initialize(GameID, APIKey, Environment))
			{
				Modio::Detail::Logger().Log(Modio::LogLevel::Error, Modio::LogCategory::Core,
											"mod.io SDK was already initialized!");
				self.complete(Modio::make_error_code(Modio::GenericError::SDKAlreadyInitialized));
				return;
			}

			Modio::Detail::Logger().Log(Modio::LogLevel::Info, Modio::LogCategory::Core,
										"Initializing mod.io services");
			yield Modio::Detail::Services::GetGlobalService<Modio::Detail::FileService>().async_Initialize(
				User, GameID, std::move(self));
			if (ec)
			{
				self.complete(ec);
				return;
			}

			Modio::Detail::Logger().Log(Modio::LogLevel::Info, Modio::LogCategory::File, "Initialized File Service");
			yield Modio::Detail::Services::GetGlobalService<Modio::Detail::HttpService>().async_Initialize(
				std::move(self));
			if (ec)
			{
				self.complete(ec);
				return;
			}

			Modio::Detail::Logger().Log(Modio::LogLevel::Info, Modio::LogCategory::Http, "Initialized Http Service");

			yield Modio::Detail::LoadModCollectionFromStorageAsync(std::move(self));

			// Init user data service
			yield Modio::Detail::Services::GetGlobalService<Modio::Detail::UserDataService>().async_Initialize(
				std::move(self));
			if (ec && ec != Modio::UserAuthError::StatusAuthTokenMissing)
			{
				self.complete(ec);
				return;
			}
			Modio::Detail::Logger().Log(Modio::LogLevel::Info, Modio::LogCategory::User,
										"Initialized User Data service");

			// other service init

			Modio::Detail::Logger().Log(Modio::LogLevel::Info, Modio::LogCategory::Core,
										"mod.io service initialization complete");

			self.complete({});
		}
	}
};
