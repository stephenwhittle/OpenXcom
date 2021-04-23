#pragma once

#include "file/FileSystemImplementation.h"
#include "modio/core/ModioBuffer.h"
#include "modio/detail/ModioStringHelpers.h"
#include "modio/detail/entities/ModioAvatar.h"
#include "modio/detail/entities/ModioImage.h"
#include "modio/detail/entities/ModioLogo.h"
#include <asio.hpp>
#include <iostream>
#include <memory>

namespace Modio
{
	/// @docpublic
	/// @brief Platform-defined user handle
	using UserHandleType = Modio::Detail::FileSystemImplementation::PlatformUserHandleType;
	namespace Detail
	{
		class FileService : public asio::detail::service_base<FileService>
		{
		public:
			explicit FileService(asio::io_context& IOService) : asio::detail::service_base<FileService>(IOService)
			{
				auto NewImplementation = std::make_shared<FileSystemImplementation>(*this);
				PlatformImplementation.swap(NewImplementation);
			}

			using implementation_type = FileSystemImplementation::IOObjectImplementationType;

			void construct(implementation_type& Implementation)
			{
				PlatformImplementation->InitializeIOObjectImplementation(Implementation);
			}

			void destroy(implementation_type& Implementation)
			{
				PlatformImplementation->DestroyIOObjectImplementation(Implementation);
			}

			void move_construct(implementation_type& Implementation, implementation_type& Other)
			{
				PlatformImplementation->MoveIOObjectImplementation(Implementation, Other);
			}

			void move_assign(implementation_type& Implementation, Modio::Detail::FileService& OtherService,
							 implementation_type& Other)
			{
				// No difference between this and move_construct for us because our application will only have a single
				// io_context and a single HttpService instance
				PlatformImplementation->MoveIOObjectImplementation(Implementation, Other);
			}

			void converting_move_construct(implementation_type& impl, Modio::Detail::FileService&,
										   implementation_type& other_impl)
			{
				move_construct(impl, other_impl);
			}

			void converting_move_assign(implementation_type& impl, Modio::Detail::FileService& other_service,
										implementation_type& other_impl)
			{
				move_assign(impl, other_service, other_impl);
			}

			void Shutdown()
			{
				PlatformImplementation->Shutdown();
			}

			template<typename CompletionHandlerType>
			auto async_Initialize(Modio::UserHandleType User, Modio::GameID GameId, CompletionHandlerType&& Handler)
			{
				return PlatformImplementation->async_Initialize(
					User, GameId, std::forward<CompletionHandlerType>(std::move(Handler)));
			}

			template<typename CompletionTokenType>
			auto async_write_some_at(implementation_type& PlatformIOObject, std::uintmax_t Offset, Modio::Detail::Buffer Buffer,
									 CompletionTokenType&& Token)
			{
				return PlatformImplementation->async_write_some_at(PlatformIOObject, Offset, std::move(Buffer),
																   std::forward<CompletionTokenType>(std::move(Token)));
			}

			template<typename CompletionTokenType>
			auto async_read_some_at(implementation_type& PlatformIOObject, std::uintmax_t Offset, std::uintmax_t Length,
									CompletionTokenType&& Token)
			{
				return PlatformImplementation->async_read_some_at(PlatformIOObject, Offset, Length,
																  std::forward<CompletionTokenType>(std::move(Token)));
			}

			template<typename CompletionTokenType>
			auto async_read_some_at(implementation_type& PlatformIOObject, std::uintmax_t Offset,
									std::uintmax_t MaxBytesToRead, Modio::Detail::DynamicBuffer Destination,
									CompletionTokenType&& Token)
			{
				return PlatformImplementation->async_read_some_at(PlatformIOObject, Offset, MaxBytesToRead, Destination,
																  std::forward<CompletionTokenType>(std::move(Token)));
			}

			template<typename CompletionTokenType>
			auto async_read(implementation_type& PlatformIOObject, std::uintmax_t MaxBytesToRead,
							Modio::Detail::DynamicBuffer Destination, CompletionTokenType&& Token)
			{
				return PlatformImplementation->async_read(PlatformIOObject, MaxBytesToRead, Destination,
														  std::forward<CompletionTokenType>(std::move(Token)));
			}

			template<typename CompletionTokenType>
			auto async_write(implementation_type& PlatformIOObject, Modio::Detail::Buffer Buffer, CompletionTokenType&& Token)
			{
				return PlatformImplementation->async_write(PlatformIOObject, std::move(Buffer),
														   std::forward<CompletionTokenType>(std::move(Token)));
			}

			template<typename CompletionTokenType>
			auto async_DeleteFolder(Modio::filesystem::path FolderPath, CompletionTokenType&& Token)
			{
				return PlatformImplementation->async_DeleteFolder(FolderPath,
																  std::forward<CompletionTokenType>(std::move(Token)));
			}

			Modio::filesystem::path UserDataFolder() const
			{
				return PlatformImplementation->UserDataFolder();
			}

			Modio::filesystem::path LocalMetadataFolder() const
			{
				return PlatformImplementation->LocalMetadataFolder();
			}

			Modio::Optional<Modio::filesystem::path> MakeTempFilePath(std::string FileName) const
			{
				return PlatformImplementation->MakeTempFilePath(FileName);
			}

			Modio::filesystem::path MakeModPath(Modio::ModID ModID) const
			{
				return PlatformImplementation->MakeModPath(ModID);
			}

			/// <summary>
			/// Get the path to a logo of a mod
			/// </summary>
			/// <param name="ModID">A valid id of a mod</param>
			/// <param name="Size">The logo size</param>
			/// <param name="OriginalFileName"></param>
			/// <returns>A valid path</returns>
			Modio::filesystem::path MakeModMediaFilePath(Modio::ModID ModID, Modio::LogoSize Size,
														 const std::string& OriginalFileName) const
			{
				return PlatformImplementation->MakeModMediaFilePath(ModID, Size, OriginalFileName);
			}

			/// <summary>
			/// Get the path to a gallery image of a mod
			/// </summary>
			/// <param name="ModID">A valid id of a mod</param>
			/// <param name="Size">The image size</param>
			/// <param name="ImageIndex">Index of the image</param>
			/// <param name="OriginalFileName"></param>
			/// <returns>A valid path</returns>
			Modio::filesystem::path MakeModMediaFilePath(Modio::ModID ModID, Modio::GallerySize Size,
														 Modio::GalleryIndex ImageIndex,
														 const std::string& OriginalFileName) const
			{
				return PlatformImplementation->MakeModMediaFilePath(ModID, Size, ImageIndex, OriginalFileName);
			}

			/// <summary>
			/// Get the directory where the cached logos will reside for a mod
			/// </summary>
			/// <param name="ModID">A valid mod id</param>
			/// <returns>The path where cached logos will reside for the mod</returns>
			Modio::filesystem::path MakeLogoFolderPath(Modio::ModID ModID) const
			{
				return PlatformImplementation->MakeLogoFolderPath(ModID);
			}

			/// <summary>
			/// Get the directory where the cached gallery images will reside for a mod
			/// </summary>
			/// <param name="ModID">A valid mod id</param>
			/// <returns>The path where cached logos will reside for the mod</returns>
			Modio::filesystem::path MakeGalleryFolderPath(Modio::ModID ModID, Modio::GalleryIndex ImageIndex) const
			{
				return PlatformImplementation->MakeGalleryFolderPath(ModID, ImageIndex);
			}

			/// <summary>
			/// Mirror of MakeUserMediaFilePath
			/// </summary>
			/// <param name="ModID">A valid id of a mod</param>
			/// <param name="Size">The logo size</param>
			/// <param name="OriginalFileName"></param>
			/// <returns>A valid path</returns>
			inline Modio::filesystem::path MakeModMediaFilePath(Modio::UserID UserId, Modio::AvatarSize AvatarSize,
																const std::string& OriginalFileName) const
			{
				return MakeUserMediaFilePath(UserId, AvatarSize, OriginalFileName);
			}

			/// <summary>
			/// Get the path to a avatar
			/// </summary>
			/// <param name="ModID">A valid id of a mod</param>
			/// <param name="Size">The logo size</param>
			/// <param name="OriginalFileName"></param>
			/// <returns>A valid path</returns>
			Modio::filesystem::path MakeUserMediaFilePath(Modio::UserID UserId, Modio::AvatarSize AvatarSize,
														  const std::string& OriginalFileName) const
			{
				return PlatformImplementation->MakeUserMediaFilePath(UserId, AvatarSize, OriginalFileName);
			}

			/// <summary>
			/// Get the directory where the cached avatars will reside for a user
			/// </summary>
			/// <param name="UserId">A valid user id</param>
			/// <returns>The path where cached avatars will reside for the user</returns>
			Modio::filesystem::path MakeAvatarFolderPath(Modio::UserID UserId) const
			{
				return PlatformImplementation->MakeAvatarFolderPath(UserId);
			}

			/// <summary>
			/// Gets the logo of a mod if one exists for the specified size
			/// </summary>
			/// <param name="ModID"></param>
			/// <param name="Size"></param>
			/// <returns></returns>
			Modio::Optional<Modio::filesystem::path> GetLogo(Modio::ModID ModID, Modio::LogoSize Size) const
			{
				return GetMediaInternal(MakeLogoFolderPath(ModID), Modio::Detail::ToString(Size));
			}

			/// <summary>
			/// Gets the avatar of a user if one exists for the specified size
			/// </summary>
			/// <param name="UserId"></param>
			/// <param name="Size"></param>
			/// <returns></returns>
			Modio::Optional<Modio::filesystem::path> GetAvatar(Modio::UserID UserId, Modio::AvatarSize Size) const
			{
				return GetMediaInternal(MakeAvatarFolderPath(UserId), Modio::Detail::ToString(Size));
			}

			Modio::Optional<Modio::filesystem::path> GetGalleryImage(Modio::ModID ModID, Modio::GallerySize Size,
																	 Modio::GalleryIndex Index) const
			{
				return GetMediaInternal(MakeGalleryFolderPath(ModID, Index), Modio::Detail::ToString(Size));
			}

			// @todo: Should all functions be able to operate on any directory anywhere or should we ensure that you
			// can only use the functions within a "sandbox" directory and don't accidentally create files outside it

			/// <summary>
			/// Create a folder on the drive
			/// </summary>
			/// <param name="FolderPath">Aboslute path of the folder</param>
			/// <returns>true if the folder already exists or was created successfully</returns>
			bool CreateFolder(const Modio::filesystem::path& FolderPath) const
			{
				return PlatformImplementation->CreateFolder(FolderPath);
			}

			/// <summary>
			/// Checks if a file exists on the drive
			/// </summary>
			/// <param name="FilePath">The path we want to check</param>
			/// <returns>True if it's a file and it exists</returns>
			bool FileExists(const Modio::filesystem::path& FilePath) const
			{
				return PlatformImplementation->FileExists(FilePath);
			}

			/// <summary>
			/// Tries to delete a file on the drive
			/// </summary>
			/// <param name="FilePath">Path to the file</param>
			/// <returns>True if it was a file and it was deleted successfully</returns>
			bool DeleteFile(const Modio::filesystem::path& FilePath) const
			{
				return PlatformImplementation->DeleteFile(FilePath);
			}

			const Modio::filesystem::path& GetRootLocalStoragePath() const
			{
				return PlatformImplementation->GetRootLocalStoragePath();
			}

		private:
			/// <summary>
			/// Gets the mod media of a user if one exists for the specified size
			/// </summary>
			/// <param name="FolderPath">The path that contains all the different sizes of the image</param>
			/// <param name="SizeSuffix">The suffix appended to the filename so we know what size the image is
			/// of</param>
			/// <returns>a unset optional either if there was a error </returns>
			Modio::Optional<Modio::filesystem::path> GetMediaInternal(const Modio::filesystem::path& FolderPath,
																	  const std::string& SizeSuffix) const
			{
				namespace fs = Modio::filesystem;
				fs::path ImageCachePath = FolderPath;
				std::error_code ec;
				auto InitialIterator = fs::directory_iterator(ImageCachePath, ec);
				if(ec)
				{
					Modio::Detail::Logger().Log(LogLevel::Error, LogCategory::File, "GetMediaInternal failed to create a directory iterator with error {}", ec.message());
					return {};
				}
				
				for (auto& Entry = InitialIterator; Entry != end(InitialIterator); Entry.increment(ec))
				{
					bool bRegularFile = Entry->is_regular_file(ec);
					if (ec || !bRegularFile)
					{
						continue;
					}

					const fs::path& FullPath = Entry->path();
					fs::path FilenameNoExtension = FullPath.filename().stem();
					if (Modio::Detail::String::EndsWith(FilenameNoExtension.u8string(), fmt::format("_{}", SizeSuffix)))
					{
						return FullPath;
					}
				}

				return {};
			}

			std::shared_ptr<FileSystemImplementation> PlatformImplementation;
		};
	} // namespace Detail
} // namespace Modio
