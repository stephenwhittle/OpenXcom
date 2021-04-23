#pragma once
#include <asio.hpp>
#include <ShlObj.h>
//#include "modio/detail/ModioSDKSessionData.h"

#include <asio/yield.hpp>
namespace Modio
{
	namespace Detail
	{
		/// @brief Default implementation of the file system initialization on MS platforms. Can be re-used on any
		/// platform where the user identifier passed into InitializeAsync is a std::string.
		class InitializeFileSystem
		{
		public:
			InitializeFileSystem(std::string NewUserID, Modio::GameID GameID, Modio::filesystem::path& CommonDataPath, Modio::filesystem::path& UserDataPath, Modio::filesystem::path& TempPath)
				:NewUserID(NewUserID),
				GameID(GameID),
				CommonDataPath(CommonDataPath),
				UserDataPath(UserDataPath),
				TempPath(TempPath)
			{

			}

			template<typename CoroType>
			void operator()(CoroType& Self, Modio::ErrorCode ec = {})
			{
				/*if (!Modio::Detail::SDKSessionData::IsInitialized())
				{
					Self.complete(Modio::make_error_code(Modio::GenericError::OperationCanceled));
					return;
				}*/
				reenter(CoroutineState)
				{
					// Get root data path in program data
					{
						PWSTR path = NULL;

						HRESULT hr = SHGetKnownFolderPath(FOLDERID_ProgramData, 0, NULL, &path);

						if (SUCCEEDED(hr))
						{
							CommonDataPath = Modio::filesystem::path(std::wstring(path));
							CommonDataPath /= fmt::format("mod.io/{}/", GameID);
							CoTaskMemFree(path);
						}
						else
						{
							CoTaskMemFree(path);
							Self.complete(Modio::make_error_code(Modio::FilesystemError::UnableToCreateFolder));
							return;
						}
					}

					// Get user data directory in AppData
					{
						Modio::filesystem::path UDF;
						PWSTR path = NULL;

						HRESULT hr = SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &path);
						std::string UserID = NewUserID;

						if (SUCCEEDED(hr))
						{
							UDF = Modio::filesystem::path(std::wstring(path));
							UDF /= Modio::filesystem::path("mod.io") / fmt::format("{}/", UserID);
							CoTaskMemFree(path);
							UserDataPath = UDF;
						}
						else
						{
							CoTaskMemFree(path);
							Self.complete(Modio::make_error_code(Modio::FilesystemError::UnableToCreateFolder));
							return;
						}
					}
					TempPath = Modio::filesystem::temp_directory_path(ec);
					Self.complete(Modio::ErrorCode {});
				}
			}

		private:
			asio::coroutine CoroutineState;
			std::string NewUserID;
			Modio::GameID GameID;
			Modio::filesystem::path& CommonDataPath;
			Modio::filesystem::path& UserDataPath;
			Modio::filesystem::path& TempPath;
		};
	} // namespace Detail
} // namespace Modio
#include <asio/unyield.hpp>