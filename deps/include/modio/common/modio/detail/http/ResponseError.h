#pragma once

#include "modio/detail/ModioJsonHelpers.h"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace Modio
{
	namespace Detail
	{
		struct FieldError
		{
			std::string Field;
			std::string Error;
		};

		struct ResponseError
		{
			std::int32_t Code;
			std::int32_t ErrorRef;
			std::string Error;
			Modio::Optional<std::vector<FieldError>> ExtendedErrorInformation;
		};

		static void from_json(const nlohmann::json& Json, Modio::Detail::ResponseError& Error)
		{
			const nlohmann::json& Subobject = Json.at("error");
			if(Subobject.is_object())
			{
				Detail::ParseSafe(Subobject, Error.Code, "code");
				Detail::ParseSafe(Subobject, Error.ErrorRef, "error_ref");
				Detail::ParseSafe(Subobject, Error.Error, "message");

#if 0
				const nlohmann::json& ExtendedErrorMessage = Json.at("errors");
				if(ExtendedErrorMessage.is_object())
				{
					Error.ExtendedErrorInformation.emplace();
					std::vector<FieldError>& Var =
						Error.ExtendedErrorInformation.value();
					Detail::ParseSafe<std::vector<FieldError>>(Subobject, Var, "errors");
				}
#endif
			}
		}
	}
}