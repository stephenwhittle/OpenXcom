#pragma once

#include "modio/core/ModioBuffer.h"
#include "modio/core/ModioServices.h"
#include "modio/detail/ModioSDKSessionData.h"
#include "modio/file/ModioFile.h"
#include "modio/file/ModioFileService.h"
#include <asio.hpp>
#include <asio/yield.hpp>
namespace Modio
{
	namespace Detail
	{
		class LoadModCollectionFromStorage
		{
		public:
			template<typename CoroType>
			void operator()(CoroType& Self, Modio::ErrorCode ec = {})
			{
				reenter(CoroutineState)
				{
					Modio::Detail::Logger().Log(Modio::LogLevel::Trace, Modio::LogCategory::ModManagement, "Loading mod collection from storage");
			
						DestinationFile = std::make_unique<Modio::Detail::File>(
							Modio::Detail::Services::GetGlobalService<Modio::Detail::FileService>().LocalMetadataFolder() /
								"state.json",
							false);

					yield DestinationFile->async_Read(DestinationFile->GetFileSize(), DataBuffer , std::move(Self));
					if (ec)
					{
						Self.complete(ec);
						return;
					}
					else
					{
						if (DataBuffer.size() > 0)
						{
							nlohmann::json StateJson = Modio::Detail::ToJson(DataBuffer);
							from_json(StateJson, Modio::Detail::SDKSessionData::GetSystemModCollection());
						}
						Modio::Detail::Logger().Log(Modio::LogLevel::Trace, Modio::LogCategory::ModManagement,
											"Mod collection loaded");
			
						Self.complete({});
					}
				}
			}

		private:
			asio::coroutine CoroutineState;
			std::unique_ptr<Modio::Detail::File> DestinationFile;
			Modio::Detail::DynamicBuffer DataBuffer;
		};

		template<typename LoadModCollectionCallback>
		auto LoadModCollectionFromStorageAsync(LoadModCollectionCallback&& OnLoadComplete)
		{
			return asio::async_compose<LoadModCollectionCallback, void(Modio::ErrorCode)>(
				LoadModCollectionFromStorage(), OnLoadComplete,
				Modio::Detail::Services::GetGlobalContext().get_executor());
		}
	} // namespace Detail
} // namespace Modio
#include <asio/unyield.hpp>