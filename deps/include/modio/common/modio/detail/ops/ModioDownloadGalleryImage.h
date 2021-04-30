#pragma once

#include "modio/core/ModioCoreTypes.h"
#include "modio/detail/entities/ModioGalleryList.h"
#include "modio/file/ModioFileService.h"

namespace Modio
{
	namespace Detail
	{
		struct GalleryImageType
		{
			GalleryImageType(Modio::ModID ModId, const Modio::Detail::GalleryList& GalleryList,
							 Modio::GallerySize GallerySize, Modio::GalleryIndex ImageIndex)
				: ModId(ModId),
				  GallerySize(GallerySize),
				  ImageIndex(ImageIndex),
				  ImageURL(GetImmageURL(GalleryList[ImageIndex], GallerySize))
			{}

			bool MakeDestinationPath(Modio::filesystem::path& OutPath) const
			{
				Modio::Detail::FileService& FileService = Modio::Detail::Services::GetGlobalService<Modio::Detail::FileService>();
				Modio::Optional<std::string> FileName = Modio::Detail::String::GetFilenameFromURL(ImageURL);
				if (FileName)
				{
					OutPath = FileService.MakeModMediaFilePath(ModId, GallerySize, ImageIndex, *FileName);
					return true;
				}
				return false;
			}

			Modio::Optional<Modio::filesystem::path> GetCachePath() const
			{
				Modio::Detail::FileService& FileService = Modio::Detail::Services::GetGlobalService<Modio::Detail::FileService>();
				return FileService.GetGalleryImage(ModId, GallerySize, ImageIndex);
			}

			const std::string& GetDownloadURL() const
			{
				return ImageURL;
			}

			Modio::ModID ModId;
			Modio::GallerySize GallerySize;
			Modio::GalleryIndex ImageIndex;
			std::string ImageURL;
		};

		// template<typename CallbackType>
		// using DownloadLogoAsync = DownloadLogoAsync<LogoImageType, CallbackType>;
	} // namespace Detail
} // namespace Modio
