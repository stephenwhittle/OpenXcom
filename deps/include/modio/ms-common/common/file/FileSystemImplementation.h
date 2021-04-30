// MIRRORED TO gdk/file/FileSystemImplementation.h, UPDATE THAT FILE IF THIS IS UPDATED
#pragma once
#include "common/detail/ops/file/DeleteFolder.h"
#include "common/detail/ops/file/ReadSomeFromFile.h"
#include "common/detail/ops/file/ReadSomeFromFileBuffered.h"
#include "common/detail/ops/file/StreamRead.h"
#include "common/detail/ops/file/StreamWrite.h"
#include "common/detail/ops/file/WriteSomeToFile.h"
#include "common/file/FileObjectImplementation.h"
#include "fmt/format.h"
#include "modio/detail/entities/ModioAvatar.h"
#include "modio/detail/entities/ModioImage.h"
#include "modio/detail/entities/ModioLogo.h"
#include "modio/detail/entities/ModioImage.h"
#include "modio/detail/file/IFileObjectImplementation.h"
#include "modio/detail/file/IFileServiceImplementation.h"
#include <memory>

namespace Modio
{
	namespace Detail
	{
		template<typename Subplatform, typename UserHandleType>
		class FileSystemImplementationBase : public Modio::Detail::IFileServiceImplementation
		{
			asio::io_context::service& OwningService;
			/// <summary>
			/// The root path for local persistent storage - all file paths are treated as relative to this root
			/// </summary>
			Modio::filesystem::path RootLocalStoragePath;
			Modio::filesystem::path UserDataPath;
			Modio::filesystem::path RootTempPath;

		public:
			/// <summary>
			/// Member typedef used by Modio::Detail::FileSystem
			/// </summary>
			using IOObjectImplementationType = std::shared_ptr<FileObjectImplementation>;
			using PlatformUserHandleType = UserHandleType;

			std::vector<std::weak_ptr<FileObjectImplementation>> OpenFileObjects;

			FileSystemImplementationBase(asio::io_context::service& OwningService) : OwningService(OwningService) {}

			/// <summary>
			/// Delegated initializer for platform implementations of IO objects
			/// </summary>
			/// <param name="PlatformIOObjectInstance"></param>
			void InitializeIOObjectImplementation(IOObjectImplementationType& PlatformIOObjectInstance)
			{
				// TODO: @modio-xdk work out a way to enforce initialization of services before IO objects
				PlatformIOObjectInstance =
					std::make_shared<FileObjectImplementation>(OwningService.get_io_context(), RootLocalStoragePath);

				OpenFileObjects.erase(std::remove_if(OpenFileObjects.begin(), OpenFileObjects.end(),
													 [](const std::weak_ptr<FileObjectImplementation>& CheckedObject) {
														 return CheckedObject.expired();
													 }),
									  OpenFileObjects.end());
				OpenFileObjects.push_back(PlatformIOObjectInstance);
			}
			void DestroyIOObjectImplementation(IOObjectImplementationType& PlatformIOObjectInstance)
			{
				if (PlatformIOObjectInstance)
				{
					PlatformIOObjectInstance->Destroy();
				}
			}

			void MoveIOObjectImplementation(IOObjectImplementationType& Implementation,
											IOObjectImplementationType& OtherImplementation)
			{
				Implementation = std::move(OtherImplementation);
			}

			void Shutdown()
			{
				for (auto FileObject : OpenFileObjects)
				{
					if (auto FileImpl = FileObject.lock())
					{
						FileImpl->CancelAll();
					}
				}
			}

			template<typename CompletionTokenType>
			auto async_Initialize(PlatformUserHandleType NewUserHandle, Modio::GameID GameID,
								  CompletionTokenType&& Token)
			{
				return asio::async_compose<CompletionTokenType, void(std::error_code)>(
					static_cast<Subplatform*>(this)->MakeInitializeStorageOp(
						NewUserHandle, GameID, RootLocalStoragePath, UserDataPath, RootTempPath),
					Token, Modio::Detail::Services::GetGlobalContext().get_executor());
			}

			template<typename CompletionTokenType>
			auto async_write_some_at(IOObjectImplementationType PlatformIOObjectInstance, std::uintmax_t Offset,
									 Modio::Detail::Buffer Buffer, CompletionTokenType&& Token)
			{
				return asio::async_compose<CompletionTokenType, void(std::error_code)>(
					WriteSomeToFile(PlatformIOObjectInstance, Offset, std::move(Buffer)), Token,
					Modio::Detail::Services::GetGlobalContext().get_executor());
			}

			template<typename CompletionTokenType>
			auto async_read_some_at(IOObjectImplementationType PlatformIOObjectInstance, std::uintmax_t Offset,
									std::uintmax_t Length, CompletionTokenType&& Token)
			{
				return asio::async_compose<CompletionTokenType,
										   void(std::error_code, Modio::Optional<Modio::Detail::Buffer>)>(
					ReadSomeFromFile(PlatformIOObjectInstance, Offset, Length), Token,
					Modio::Detail::Services::GetGlobalContext().get_executor());
			}

			template<typename CompletionTokenType>
			auto async_read_some_at(IOObjectImplementationType PlatformIOObjectInstance, std::uintmax_t Offset,
									std::uintmax_t MaxBytesToRead, Modio::Detail::DynamicBuffer Destination,
									CompletionTokenType&& Token)
			{
				return asio::async_compose<CompletionTokenType, void(std::error_code)>(
					ReadSomeFromFileBuffered(PlatformIOObjectInstance, Modio::FileOffset(Offset), MaxBytesToRead,
											 Destination),
					Token, Modio::Detail::Services::GetGlobalContext().get_executor());
			}

			template<typename CompletionTokenType>
			auto async_read(IOObjectImplementationType PlatformIOObjectInstance, std::uintmax_t MaxBytesToRead,
							Modio::Detail::DynamicBuffer Destination, CompletionTokenType&& Token)
			{
				return asio::async_compose<CompletionTokenType, void(std::error_code)>(
					ReadSomeFromFileBuffered(PlatformIOObjectInstance, PlatformIOObjectInstance->Tell(), MaxBytesToRead,
											 Destination),
					Token, Modio::Detail::Services::GetGlobalContext().get_executor());
			}

			template<typename CompletionTokenType>
			auto async_write(IOObjectImplementationType PlatformIOObjectInstance, Modio::Detail::Buffer Buffer,
							 CompletionTokenType&& Token)
			{
				return asio::async_compose<CompletionTokenType, void(std::error_code)>(
					StreamWrite(PlatformIOObjectInstance, std::move(Buffer)), Token,
					Modio::Detail::Services::GetGlobalContext().get_executor());
			}

			template<typename CompletionTokenType>
			auto async_DeleteFolder(Modio::filesystem::path FolderPath, CompletionTokenType&& Token)
			{
				return asio::async_compose<CompletionTokenType, void(std::error_code)>(
					DeleteFolderOp(FolderPath), Token, Modio::Detail::Services::GetGlobalContext().get_executor());
			}

			// do we need to maintain a cache of temporary files?
			Modio::filesystem::path MakeTempFilePath(std::string FileName) const
			{
				return RootTempPath / FileName;
			}

			Modio::filesystem::path MakeModPath(Modio::ModID ModID) const
			{
				return RootLocalStoragePath / fmt::format("mods/{}", ModID);
			}

			Modio::filesystem::path MakeModMediaFilePath(Modio::ModID ModID, Modio::LogoSize Size,
														 const std::string& OriginalFilename) const
			{
				const Modio::filesystem::path OriginalFilePath = OriginalFilename;
				return RootLocalStoragePath / fmt::format("{}/{}_{}{}", MakeLogoFolderPath(ModID).u8string(),
														  OriginalFilePath.stem().u8string(),
														  Modio::Detail::ToString(Size),
														  OriginalFilePath.extension().u8string());
			}

			Modio::filesystem::path MakeModMediaFilePath(Modio::ModID ModID, Modio::GallerySize Size,
														 Modio::GalleryIndex ImageIndex,
														 const std::string& OriginalFileName) const
			{
				const Modio::filesystem::path OriginalFilePath = OriginalFileName;
				return RootLocalStoragePath / MakeGalleryFolderPath(ModID, ImageIndex) /
					   fmt::format("{}_{}{}", OriginalFilePath.stem().u8string(), Modio::Detail::ToString(Size),
								   OriginalFilePath.extension().u8string());
			}

			Modio::filesystem::path MakeLogoFolderPath(Modio::ModID ModID) const
			{
				// @todonow: Change this to temporary storage
				return RootLocalStoragePath / fmt::format("cache/mods/{}/logos/", ModID);
			}

			Modio::filesystem::path MakeGalleryFolderPath(Modio::ModID ModID, int ImageIndex) const
			{
				// @todonow: Change this to temporary storage
				return RootLocalStoragePath / fmt::format("cache/mods/{}/gallery/{}/", ModID, ImageIndex);
			}

			Modio::filesystem::path MakeUserMediaFilePath(Modio::UserID UserId, Modio::AvatarSize Size,
														  const std::string& OriginalFilename) const
			{
				const Modio::filesystem::path OriginalFilePath = OriginalFilename;
				return RootLocalStoragePath / MakeAvatarFolderPath(UserId) /
					   fmt::format("{}_{}{}", OriginalFilePath.stem().u8string(), Modio::Detail::ToString(Size),
								   OriginalFilePath.extension().u8string());
			}

			Modio::filesystem::path MakeAvatarFolderPath(Modio::UserID UserId) const
			{
				// @todonow: Change this to temporary storage
				return RootLocalStoragePath / fmt::format("cache/users/{}/avatars/", UserId);
			}

			bool CreateFolder(const Modio::filesystem::path& FolderPath) const
			{
				std::error_code ec;
				// Using function calling taking ec as it doesn't throw when failing
				bool Result = Modio::filesystem::create_directories(FolderPath, ec);

				// @todo: Right now we are discarding relevant information about the error of why the path can't be
				// created take a decision about how we should get that information back in a good way

				if (!Result && !ec)
				{
					// This is a special case, this means that the directory wasn't created as it's already there
					// so let's call that a success
					return true;
				}

				// Error condition, print error
				if (!Result)
				{
					Modio::Detail::Logger().Log(LogLevel::Warning, LogCategory::File,
												"Failed to create directory: {}, with code: {} and message {}",
												FolderPath.u8string(), ec.value(), ec.message());
				}

				return Result;
			}

			bool FileExists(const Modio::filesystem::path& FilePath) const
			{
				namespace fs = Modio::filesystem;

				std::error_code ec;
				if (fs::exists(FilePath, ec) && !ec)
				{
					return fs::is_regular_file(FilePath, ec) && !ec;
				}

				return false;
			}

			bool DeleteFile(const Modio::filesystem::path& FilePath) const
			{
				namespace fs = Modio::filesystem;

				std::error_code ec;
				if (!fs::is_regular_file(FilePath, ec) && !ec)
				{
					Modio::Detail::Logger().Log(
						LogLevel::Warning, LogCategory::File,
						"Failed to delete file: {}, not a regular file. Additional information: {}",
						FilePath.u8string(), ec ? ec.message() : "");
					return false;
				}
				if (!Modio::filesystem::remove(FilePath, ec) || ec)
				{
					Modio::Detail::Logger().Log(LogLevel::Warning, LogCategory::File,
												"Failed to delete file: {}, with code {} and message: \"{}\"",
												FilePath.u8string(), ec.value(), ec.message());
					return false;
				}

				return true;
			}

			const Modio::filesystem::path& UserDataFolder() const
			{
				return UserDataPath;
			}

			virtual const Modio::filesystem::path& LocalMetadataFolder() const override
			{
				// Adding emptry string at end forces metadata to be treated as a directory
				static const Modio::filesystem::path MetadataFolder = RootLocalStoragePath / "metadata" / "";
				return MetadataFolder;
			}

			const Modio::filesystem::path& GetRootLocalStoragePath() const override
			{
				return RootLocalStoragePath;
			}
		};
	} // namespace Detail
} // namespace Modio
