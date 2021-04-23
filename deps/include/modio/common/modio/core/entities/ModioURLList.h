#pragma once

#include "modio/core/entities/ModioList.h"
#include <vector>
#include <string>

namespace Modio
{
	using URLList = List<std::vector,std::string>;

	class YoutubeURLList : public URLList
	{
		friend inline void from_json(const nlohmann::json& Json, Modio::YoutubeURLList& YoutubeURLList);
	};

	class SketchfabURLList : public URLList
	{
		friend inline void from_json(const nlohmann::json& Json, Modio::SketchfabURLList& SketchfabURLList);	
	};

	void from_json(const nlohmann::json& Json, Modio::YoutubeURLList& YoutubeURLList)
	{
		Detail::ParseSafe(Json, YoutubeURLList.InternalList, "youtube");
	}

	void from_json(const nlohmann::json& Json, Modio::SketchfabURLList& SketchfabURLList)
	{
		Detail::ParseSafe(Json, SketchfabURLList.InternalList, "sketchfab");
	}

	inline void to_json(nlohmann::json& Json, const Modio::YoutubeURLList& YoutubeURLList)
	{
		Json = nlohmann::json {{"youtube", YoutubeURLList.GetRawList()}};
	}

	inline void to_json(nlohmann::json& Json, const Modio::SketchfabURLList& SketchfabURLList)
	{
		Json = nlohmann::json {{"sketchfab", SketchfabURLList.GetRawList()}};
	}

}