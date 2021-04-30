#pragma once
#include "ModioGeneratedVariables.h"
#include "modio/detail/ModioDefines.h"

#include "modio/core/entities/ModioFileMetadata.h"
#include "modio/core/entities/ModioModStats.h"
#include "modio/core/entities/ModioURLList.h"
#include "modio/core/entities/ModioUser.h"
#include "modio/detail/ModioJsonHelpers.h"
#include "modio/detail/entities/ModioGalleryList.h"
#include <string>
#include <vector>

namespace Modio
{
	enum class MaturityOption : std::uint8_t
	{
		None = 0,
		Alcohol = 1,
		Drugs = 2,
		Violence = 4,
		Explicit = 8
	};

	// @todo: If ModTag and Metadata is used somewhere else than in ModInfo, then migrate them to their own headers
	struct ModTag
	{
		std::string Tag;
	};

	// Migrate: std::vector<Modio::ModTag> Tags to a custom class that encapsulates std::map helpers to fetch Values as
	// something like this: Metadata.Get<int32>( "OptimizedFor" );
	struct Metadata
	{
		std::string Key;
		std::string Value;
	};

	inline void from_json(const nlohmann::json& Json, Modio::Metadata& Metadata)
	{
		Detail::ParseSafe(Json, Metadata.Key, "metakey");
		Detail::ParseSafe(Json, Metadata.Value, "metavalue");
	}

	inline void to_json(nlohmann::json& Json, const Modio::Metadata& Metadata)
	{
		Json = nlohmann::json {{"metakey", Metadata.Key}, {"metavalue", Metadata.Value}};
	}

	inline void from_json(const nlohmann::json& Json, Modio::ModTag& ModTag)
	{
		Detail::ParseSafe(Json, ModTag.Tag, "name");
	}

	inline void to_json(nlohmann::json& Json, const Modio::ModTag& Tag)
	{
		Json = nlohmann::json {{"name", Tag.Tag}};
	}
	/// @docpublic
	/// @brief Full mod profile including current release information, media links, and stats
	struct ModInfo
	{
		/// @brief Unique Mod ID
		Modio::ModID ModId;

		/// @brief Name of the mod
		std::string ProfileName;
		/// @brief Summary of the mod
		std::string ProfileSummary;
		/// @brief Detailed description in HTML format
		std::string ProfileDescription;
		/// @brief Detailed description in plaintext
		std::string ProfileDescriptionPlaintext;
		/// @brief URL to the mod profile
		std::string ProfileURL;
		/// @brief Information on the user who submitted the mod
		Modio::User ProfileSubmittedBy;
		/// @brief Unix timestamp of the date the mod was registered
		std::int64_t ProfileDateAdded;
		/// @brief Unix timestamp of the date the mod was updated
		std::int64_t ProfileDateUpdated;
		/// @brief Unix timestamp of the date the mod was marked live
		std::int64_t ProfileDateLive;
		/// @brief Flags for maturity options
		///	* Maturity options flagged by the mod developer, this is only relevant if the parent game allows mods to
		///	* be labeled as mature.
		///	*
		///	* 0 = None set (default)
		///	* 1 = Alcohol
		///	* 2 = Drugs
		///	* 4 = Violence
		///	* 8 = Explicit
		std::uint8_t ProfileMaturityOption;

		std::string MetadataBlob;
		/// @brief Information about the mod's most recent public release
		Modio::FileMetadata FileInfo;
		/// @brief Arbitrary key-value metadata set for this mod
		std::vector<Modio::Metadata> MetadataKvp;
		/// @brief Tags this mod has set
		std::vector<Modio::ModTag> Tags;

		/// @brief Number of images in the mod's media gallery
		std::size_t NumGalleryImages;

		/// @brief List of youtube links provided by the creator of the mod
		Modio::YoutubeURLList YoutubeURLs;
		/// @brief List of sketchfab links provided by the creator of the mod
		Modio::SketchfabURLList SketchfabURLs;
		/// @brief Stats and rating information for the mod
		Modio::ModStats Stats;
	};

	static void from_json(const nlohmann::json& Json, Modio::ModInfo& ModInfo)
	{
		Detail::ParseSafe(Json, ModInfo.ModId, "id");

		{
			Detail::ParseSafe(Json, ModInfo.FileInfo, "modfile");
			Detail::ParseSafe(Json, ModInfo.MetadataBlob, "metadata_blob");
			Detail::ParseSafe(Json, ModInfo.MetadataKvp, "metadata_kvp");
			Detail::ParseSafe(Json, ModInfo.Tags, "tags");
		}

		{
			Detail::ParseSafe(Json, ModInfo.ProfileDateAdded, "date_added");
			Detail::ParseSafe(Json, ModInfo.ProfileDateLive, "date_live");
			Detail::ParseSafe(Json, ModInfo.ProfileDateUpdated, "date_updated");
			Detail::ParseSafe(Json, ModInfo.ProfileDescription, "description");
			Detail::ParseSafe(Json, ModInfo.ProfileDescriptionPlaintext, "description_plaintext");
			Detail::ParseSafe(Json, ModInfo.ProfileMaturityOption, "maturity_option");
			Detail::ParseSafe(Json, ModInfo.ProfileName, "name");
			Detail::ParseSafe(Json, ModInfo.ProfileURL, "profile_url");
			Detail::ParseSafe(Json, ModInfo.ProfileSubmittedBy, "submitted_by");
			Detail::ParseSafe(Json, ModInfo.ProfileSummary, "summary");
		}

		{
			Detail::ParseSafe(Json, ModInfo.Stats, "stats");
		}

		{
			Detail::ParseSafe(Json, ModInfo.YoutubeURLs, "media");
			Detail::ParseSafe(Json, ModInfo.SketchfabURLs, "media");
		}

		{
			// @todo: Inefficient: This parses out much more data than needed just to throw it away
			Modio::Detail::GalleryList Gallery;
			if( Detail::ParseSafe(Json, Gallery, "media"))
			{
				ModInfo.NumGalleryImages = Gallery.Size();
			}

		}
	}

	static void to_json(nlohmann::json& Json, const Modio::ModInfo& Info)
	{
		Json = nlohmann::json {{"id", Info.ModId},
							   {"modfile", Info.FileInfo},
							   {"metadata_blob", Info.MetadataBlob},
							   {"metadata_kvp", Info.MetadataKvp},
							   {"tags", Info.Tags},
							   {"date_added", Info.ProfileDateAdded},
							   {"date_live", Info.ProfileDateLive},
							   {"date_updated", Info.ProfileDateUpdated},
							   {"description", Info.ProfileDescription},
							   {"description_plaintext", Info.ProfileDescriptionPlaintext},
							   {"maturity_option", Info.ProfileMaturityOption},
							   {"name", Info.ProfileName},
							   {"profile_url", Info.ProfileURL},
							   {"submitted_by", Info.ProfileSubmittedBy},
							   {"summary", Info.ProfileSummary},
							   {"stats", Info.Stats},
							   {"media", Info.YoutubeURLs}};
	}

} // namespace Modio
