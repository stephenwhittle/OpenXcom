#pragma once
#include "modio/cache/ModioCacheService.h"
#include "modio/core/ModioServices.h"
#include "modio/detail/ModioSDKSessionData.h"
#include "modio/detail/ops/LoadModCollectionFromStorage.h"
#include "modio/file/ModioFileService.h"
#include "modio/http/ModioHttpService.h"
#include "modio/userdata/ModioUserDataService.h"

#include "modio/detail/AsioWrapper.h"
#include <asio/yield.hpp>

class ServiceInitializationOp
{
	asio::coroutine CoroutineState;
	Modio::GameID GameID;
	Modio::ApiKey APIKey;
	Modio::Environment Environment;
	Modio::UserHandleType User;

public:
	ServiceInitializationOp(Modio::GameID GameID, Modio::ApiKey APIKey, Modio::Environment Environment,
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
			yield Modio::Detail::Services::GetGlobalService<Modio::Detail::FileService>().InitializeAsync(
				User, GameID, std::move(self));
			if (ec)
			{
				Modio::Detail::SDKSessionData::Deinitialize();
				self.complete(ec);
				return;
			}

			Modio::Detail::Logger().Log(Modio::LogLevel::Info, Modio::LogCategory::File, "Initialized File Service");
			yield Modio::Detail::Services::GetGlobalService<Modio::Detail::HttpService>().InitializeAsync(
				std::move(self));
			if (ec)
			{
				Modio::Detail::SDKSessionData::Deinitialize();
				self.complete(ec);
				return;
			}

			Modio::Detail::Logger().Log(Modio::LogLevel::Info, Modio::LogCategory::Http, "Initialized Http Service");

			yield Modio::Detail::LoadModCollectionFromStorageAsync(std::move(self));

			// Init user data service
			yield Modio::Detail::Services::GetGlobalService<Modio::Detail::UserDataService>().InitializeAsync(
				std::move(self));
			if (ec && ec != Modio::UserAuthError::StatusAuthTokenMissing)
			{
				Modio::Detail::SDKSessionData::Deinitialize();

				self.complete(ec);
				return;
			}
			Modio::Detail::Logger().Log(Modio::LogLevel::Info, Modio::LogCategory::User,
										"Initialized User Data service");
			if (!Modio::Detail::SDKSessionData::GetLastUsedModDirectory().has_value())
			{
				Modio::Detail::SDKSessionData::SetDefaultModDirectory(
					Modio::Detail::Services::GetGlobalService<Modio::Detail::FileService>().LocalMetadataFolder());
			}

			Modio::Detail::Logger().Log(Modio::LogLevel::Info, Modio::LogCategory::Core,
										"mod.io service initialization complete");

			Modio::Detail::SDKSessionData::ConfirmInitialize();

			self.complete({});
		}
	}
};
