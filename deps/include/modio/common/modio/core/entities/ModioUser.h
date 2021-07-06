#pragma once
#include "ModioGeneratedVariables.h"
#include "modio/core/ModioCoreTypes.h"
#include "modio/detail/ModioJsonHelpers.h"
#include <string>

namespace Modio
{
	/// @docpublic
	/// @brief Object representing a mod.io user profile
	struct User
	{
		
		/// @brief Unique id for the user
		Modio::UserID UserId;

		/// @brief Username of the user
		std::string Username;

		/// @brief Unix timestamp the user was last online
		std::int64_t DateOnline;

		/// @brief URL of the user's mod.io profile
		std::string ProfileUrl;
	};

	inline void from_json(const nlohmann::json& Json, Modio::User& User)
	{
		Detail::ParseSafe(Json, User.UserId, "id");
		Detail::ParseSafe(Json, User.Username, "username");
		Detail::ParseSafe(Json, User.DateOnline, "date_online");
		Detail::ParseSafe(Json, User.ProfileUrl, "profile_url");
	}
	inline void to_json(nlohmann::json& Json, const Modio::User& User)
	{
		Json = nlohmann::json {{"id", User.UserId},
							   {"username", User.Username},
							   {"date_online", User.DateOnline},
							   {"profile_url", User.ProfileUrl}};
	}

} // namespace Modio