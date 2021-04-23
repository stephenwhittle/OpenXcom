#pragma once
#include "ModioGeneratedVariables.h"
#include "modio/core/ModioFilesystem.h"
#include "modio/core/ModioStdTypes.h"
#include <cstdint>
#include <vector>
namespace Modio
{
	namespace Detail
	{
		class ArchiveFileImplementation
		{
		public:
			enum class CompressionMethod
			{

			};

			struct ArchiveEntry
			{
				CompressionMethod Compression;
				Modio::filesystem::path FilePath;
				std::uintmax_t FileOffset;
				std::uintmax_t CompressedSize;
			};

		private:
			// Central directory data structure
			// Modio::optional<ZipCentralDirectory> ArchiveEntries;

			std::vector<ArchiveEntry> ArchiveEntries;

		public:
			MODIO_IMPL void AddEntry(std::string FileName, std::uintmax_t FileOffset, std::uintmax_t CompressedSize,
						  CompressionMethod Compression);
			MODIO_IMPL void AddEntry(ArchiveEntry Entry);

			Modio::filesystem::path FilePath;
			std::uintmax_t ZipMagicOffset;
			std::uint16_t NumberOfRecords;
			std::uint32_t CentralDirectorySize;
			std::uint32_t CentralDirectoryOffset;
			Modio::FileSize TotalExtractedSize;
			MODIO_IMPL std::uintmax_t GetNumberOfEntries();

			MODIO_IMPL std::vector<ArchiveEntry>::iterator begin();

			MODIO_IMPL std::vector<ArchiveEntry>::iterator end();
		};
	}
}

#ifndef MODIO_SEPARATE_COMPILATION
#include "ArchiveFileImplementation.ipp"
#endif