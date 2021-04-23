#pragma once

namespace Modio
{
	namespace Detail
	{
		class IFileServiceImplementation
		{
			public:
			virtual const Modio::filesystem::path& LocalMetadataFolder() const = 0;
			virtual Modio::filesystem::path MakeTempFilePath(std::string Filename) const = 0;
			virtual Modio::filesystem::path MakeModPath(Modio::ModID ID) const = 0;
			virtual Modio::filesystem::path MakeModMediaFilePath(Modio::ModID ID, Modio::LogoSize Size,
																 const std::string& OriginalFilename) const = 0;
			//TODO: @modio-core is this necessary? mod media but avatar size??
			/*virtual Modio::filesystem::path MakeModMediaFilePath(Modio::ModID ID, Modio::AvatarSize Size,
																 const std::string& OriginalFilename) const = 0;*/
			virtual Modio::filesystem::path MakeLogoFolderPath(Modio::ModID ID) const = 0;
			virtual Modio::filesystem::path MakeGalleryFolderPath(Modio::ModID ID, Modio::GalleryIndex ImageIndex) const = 0;
			virtual Modio::filesystem::path MakeUserMediaFilePath(Modio::UserID ID, Modio::AvatarSize Size,
																  const std::string& OriginalFilename) const = 0;
			virtual Modio::filesystem::path MakeAvatarFolderPath(Modio::UserID ID) const = 0;

			virtual bool CreateFolder(const Modio::filesystem::path& FolderPath) const = 0;
			virtual bool FileExists(const Modio::filesystem::path& FilePath) const = 0;
			virtual bool DeleteFile(const Modio::filesystem::path& FilePath) const = 0;
			virtual const Modio::filesystem::path& GetRootLocalStoragePath() const = 0;
		};
	}
}