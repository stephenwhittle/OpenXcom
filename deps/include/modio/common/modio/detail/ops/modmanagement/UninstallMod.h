#pragma once
#include "modio/core/ModioServices.h"
#include "modio/file/ModioFileService.h"
#include "modio/core/ModioStdTypes.h"
#include "modio/core/ModioFilesystem.h"

#include <asio.hpp>

#include <asio/yield.hpp>
namespace Modio
{
	namespace Detail
	{
		class UninstallMod
		{
		public:
			UninstallMod(Modio::ModID ModId) : ModId(ModId) {};
			template<typename CoroType>
			void operator()(CoroType& Self, Modio::ErrorCode ec = {})
			{
				reenter(CoroutineState)
				{
					InstallPath = Modio::Detail::Services::GetGlobalService<Modio::Detail::FileService>().MakeModPath(ModId);
					yield Modio::Detail::Services::GetGlobalService<Modio::Detail::FileService>().async_DeleteFolder(
						InstallPath, std::move(Self));
					// If the directory never existed, then don't call it a failure if we couldn't delete the files
					if(ec && ec != std::errc::no_such_file_or_directory)
					{
						// Mod will still be considered as installed as something went wrong when deleting the file
						Self.complete(ec);
						return;
					}
					
					Modio::Detail::SDKSessionData::GetSystemModCollection().RemoveMod(ModId);
					
					Self.complete({});
				}
			}

		private:
			Modio::filesystem::path InstallPath;	
			Modio::ModID ModId;
			asio::coroutine CoroutineState;
		};

		template<typename UninstallDoneCallback>
		auto async_UninstallMod(Modio::ModID Mod, UninstallDoneCallback&& OnUninstallComplete)
		{
			return asio::async_compose<UninstallDoneCallback, void(Modio::ErrorCode)>(
				UninstallMod(Mod), OnUninstallComplete, Modio::Detail::Services::GetGlobalContext().get_executor());
		}
	} // namespace Detail
} // namespace Modio
#include <asio/unyield.hpp>