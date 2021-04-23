#pragma once

#include "modio/core/entities/ModioPagedResult.h"
#include "modio/core/entities/ModioList.h"
#include "modio/core/entities/ModioFileMetadata.h"
#include "modio/detail/ModioJsonHelpers.h"

#include <vector>

namespace Modio
{
	/// @docpublic
	/// @brief Collection of FileMetadata objects representing mod file updates
	class ModDetails : public PagedResult, public List<std::vector, FileMetadata>
	{
		friend inline void from_json(const nlohmann::json& Json, Modio::ModDetails& ModDetails);
	};

	void from_json(const nlohmann::json& Json, Modio::ModDetails& ModDetails)
	{
		from_json(Json, static_cast<Modio::PagedResult&>(ModDetails));

		Modio::Detail::ParseSafe(Json, ModDetails.InternalList, "data");
	}
}