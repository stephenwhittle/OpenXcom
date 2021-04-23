#pragma once
#include "modio/core/ModioBuffer.h"
#include "modio/core/ModioServices.h"
#include "modio/core/ModioStdTypes.h"
#include "modio/detail/ModioSDKSessionData.h"
#include "modio/file/ModioFile.h"
#include "modio/file/ModioFileService.h"
#include <asio.hpp>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>

namespace Modio
{
	namespace Detail
	{
#include <asio/yield.hpp>
		class SaveUserDataToStorage
		{
		public:
			SaveUserDataToStorage()
			{
				LocalState = std::make_shared<Impl>();

				LocalState->UserDataBuffer =
					std::make_unique<Modio::Detail::Buffer>(std::move(Modio::Detail::SDKSessionData::SerializeUserData()));
				LocalState->UserDataFile = std::make_unique<Modio::Detail::File>(
					Modio::Detail::Services::GetGlobalService<Modio::Detail::FileService>().UserDataFolder() / "user.json",
					true);
				
			}
			template<typename CoroType>
			void operator()(CoroType& Self, Modio::ErrorCode ec = {})
			{
				reenter(CoroutineState)
				{
					yield LocalState->UserDataFile->async_Write(std::move(*LocalState->UserDataBuffer),
																std::move(Self));

					Self.complete(ec);
					return;
				}
			}

		private:
			asio::coroutine CoroutineState;
			struct Impl
			{
				std::unique_ptr<Modio::Detail::File> UserDataFile;
				std::unique_ptr<Modio::Detail::Buffer> UserDataBuffer;
			};

			Modio::StableStorage<Impl> LocalState;
		};
#include <asio/unyield.hpp>

	} // namespace Detail
} // namespace Modio