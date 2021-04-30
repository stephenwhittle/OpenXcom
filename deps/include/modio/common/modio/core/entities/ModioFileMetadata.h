#pragma once

#include "modio/core/ModioCoreTypes.h"
#include "modio/detail/ModioJsonHelpers.h"
#include "nlohmann/json.hpp"

#include <string>

namespace Modio
{
	/// @docpublic
	/// @brief Metadata for a release archive for a mod
	struct FileMetadata
	{
		enum class VirusScanStatus : std::uint8_t
		{
			NotScanned = 0,
			ScanComplete = 1,
			InProgress = 2,
			TooLargeToScan = 3,
			FileNotFound = 4,
			ErrorScanning = 5
		};

		enum class VirusStatus : std::uint8_t
		{
			NoThreat = 0,
			Malicious = 1
		};

		///@brief Unique modfile id.
		Modio::FileMetadataID MetadataId; // @todo: Reiterate this name
		///@brief Unique mod id.
		Modio::ModID ModId;
		///@brief Unix timestamp of date file was added.
		std::int64_t DateAdded;
		///@brief Current virus scan status of the file. For newly added files that have yet to be scanned this field
		/// will change frequently until a scan is complete
		VirusScanStatus CurrentVirusScanStatus;
		///@brief Was a virus detected?
		VirusStatus CurrentVirusStatus;
		///@brief Size of the file in bytes.
		std::int64_t Filesize;
		///@brief Filename including extension.
		std::string Filename;
		///@brief Release version this file represents.
		std::string Version;
		///@brief Changelog for the file.
		std::string Changelog;
		///@brief Metadata stored by the game developer for this file.
		std::string MetadataBlob;
	};

	NLOHMANN_JSON_SERIALIZE_ENUM(FileMetadata::VirusScanStatus, {{FileMetadata::VirusScanStatus::NotScanned, 0},
																 {FileMetadata::VirusScanStatus::ScanComplete, 1},
																 {FileMetadata::VirusScanStatus::InProgress, 2},
																 {FileMetadata::VirusScanStatus::TooLargeToScan, 3},
																 {FileMetadata::VirusScanStatus::FileNotFound, 4},
																 {FileMetadata::VirusScanStatus::ErrorScanning, 5}})

	NLOHMANN_JSON_SERIALIZE_ENUM(FileMetadata::VirusStatus,
								 {{FileMetadata::VirusStatus::NoThreat, 0}, {FileMetadata::VirusStatus::Malicious, 1}})

	static void from_json(const nlohmann::json& Json, Modio::FileMetadata& FileMetadata)
	{
		std::string debug = Json.dump();
		Detail::ParseSafe(Json, FileMetadata.MetadataId, "id");
		Detail::ParseSafe(Json, FileMetadata.ModId, "mod_id");
		Detail::ParseSafe(Json, FileMetadata.DateAdded, "date_added");
		Detail::ParseSafe(Json, FileMetadata.CurrentVirusScanStatus, "virus_status");
		Detail::ParseSafe(Json, FileMetadata.CurrentVirusStatus, "virus_positive");
		Detail::ParseSafe(Json, FileMetadata.Filesize, "filesize");
		Detail::ParseSafe(Json, FileMetadata.Filename, "filename");
		Detail::ParseSafe(Json, FileMetadata.Version, "version");
		Detail::ParseSafe(Json, FileMetadata.Changelog, "changelog");
		Detail::ParseSafe(Json, FileMetadata.MetadataBlob, "metadata_blob");
	}

	static void to_json(nlohmann::json& Json, const Modio::FileMetadata& FileMetadata)
	{
		Json = {{"id", FileMetadata.MetadataId},
				{"mod_id", FileMetadata.ModId},
				{"date_added", FileMetadata.DateAdded},
				{"virus_status", FileMetadata.CurrentVirusScanStatus},
				{"virus_positive", FileMetadata.CurrentVirusStatus},
				{"filesize", FileMetadata.Filesize},
				{"filename", FileMetadata.Filename},
				{"version", FileMetadata.Version},
				{"changelog", FileMetadata.Changelog},
				{"metadata_blob", FileMetadata.MetadataBlob}};
	}
} // namespace Modio
