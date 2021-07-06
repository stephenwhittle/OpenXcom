#ifdef MODIO_SEPARATE_COMPILATION
	#include "modio/core/entities/ModioModInfo.h"
#endif

#include "modio/detail/ModioJsonHelpers.h"
#include <nlohmann/json.hpp>
namespace Modio
{
	void from_json(const nlohmann::json& Json, Modio::Metadata& Metadata)
	{
		Detail::ParseSafe(Json, Metadata.Key, "metakey");
		Detail::ParseSafe(Json, Metadata.Value, "metavalue");
	}

	void to_json(nlohmann::json& Json, const Modio::Metadata& Metadata)
	{
		Json = nlohmann::json {{"metakey", Metadata.Key}, {"metavalue", Metadata.Value}};
	}

	void from_json(const nlohmann::json& Json, Modio::ModTag& ModTag)
	{
		Detail::ParseSafe(Json, ModTag.Tag, "name");
	}

	void to_json(nlohmann::json& Json, const Modio::ModTag& Tag)
	{
		Json = nlohmann::json {{"name", Tag.Tag}};
	}

	void from_json(const nlohmann::json& Json, Modio::ModInfo& ModInfo)
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
			if (Detail::ParseSafe(Json, Gallery, "media"))
			{
				ModInfo.NumGalleryImages = Gallery.Size();
			}
		}
	}

	void to_json(nlohmann::json& Json, const Modio::ModInfo& Info)
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
							   {"media", Info.YoutubeURLs}
		};
	}

}