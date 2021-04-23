#pragma once

#include "modio/core/ModioStdTypes.h"
#include "modio/detail/ModioJsonHelpers.h"

namespace Modio::Detail::Schema
{
	struct AccessTokenObject
	{
		std::int32_t HttpResponseCode;
		std::string AccessToken;
		Modio::Timestamp DateExpires;
	};

	inline void from_json(const nlohmann::json& Json, AccessTokenObject& AccessToken)
	{
		Modio::Detail::ParseSafe(Json, AccessToken.HttpResponseCode, "code");
		Modio::Detail::ParseSafe(Json, AccessToken.AccessToken, "access_token");
		Modio::Detail::ParseSafe(Json, AccessToken.DateExpires, "date_expires");
	}
}