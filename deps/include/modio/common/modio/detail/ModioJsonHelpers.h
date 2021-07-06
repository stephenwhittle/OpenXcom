#pragma once

#include "modio/core/ModioBuffer.h"
#include "modio/detail/FilesystemWrapper.h"
#include <nlohmann/json.hpp>

namespace Modio
{
	//Has to live here for ADL to work
	namespace ghc
	{
		namespace filesystem
		{
			inline void to_json(nlohmann::json& Json, const Modio::filesystem::path& Path)
			{
				return to_json(Json, Path.u8string());
			}

			inline void from_json(const nlohmann::json& Json, Modio::filesystem::path& Path)
			{
				Path = Modio::filesystem::path(Json.get<std::string>());
			}
		} // namespace filesystem
	} // namespace ghc

	namespace Detail
	{
		template<typename T>
		inline bool ParseSafe(const nlohmann::json& Json, T& OutVar, const std::string& Key)
		{
			if (Json.contains(Key) && !Json.at(Key).is_null())
			{
				Json.at(Key).get_to<T>(OutVar);
				return true;
			}
			else
			{
				return false;
			}
		}

		template<>
		inline bool ParseSafe<Modio::filesystem::path>(const nlohmann::json& Json, Modio::filesystem::path& OutVar,
													   const std::string& Key)
		{
			if (Json.contains(Key) && !Json.at(Key).is_null())
			{
				std::string PathString = Json.at(Key).get<std::string>();
				OutVar = Modio::filesystem::path(PathString);
				return true;
			}
			else
			{
				return false;
			}
		}

		template<typename T>
		inline void from_json(const nlohmann::json& Json, std::shared_ptr<T>& Object)
		{
			if (Object)
			{
				from_json(Json, *Object);
			}
		}

		inline bool GetSubobjectSafe(const nlohmann::json& Json, const std::string& SubobjectKey,
									 nlohmann::json& OutSubobjectJson)
		{
			if (Json.contains(SubobjectKey))
			{
				const nlohmann::json& Subobject = Json.at(SubobjectKey);
				if (!Subobject.is_null())
				{
					OutSubobjectJson = Subobject;
					return true;
				}
			}

			return false;
		}

		inline bool ParseArraySizeSafe(const nlohmann::json& Json, std::size_t& OutVar, const std::string& Key)
		{
			if (Json.contains(Key) && !Json.at(Key).is_null())
			{
				OutVar = Json.at(Key).size();
				return true;
			}
			else
			{
				return false;
			}
		}

		template<typename T>
		inline void ParseSubobjectSafe(const nlohmann::json& Json, T& OutVar, const std::string& SubobjectKey,
									   const std::string& Key)
		{
			if (Json.contains(SubobjectKey))
			{
				const nlohmann::json& Subobject = Json.at(SubobjectKey);
				if (!Subobject.is_null())
				{
					ParseSafe(Subobject, OutVar, Key);
				}
			}
		}

		inline nlohmann::json ToJson(Modio::Detail::Buffer InBuffer)
		{
			return nlohmann::json::parse(InBuffer.Data());
		}

		inline nlohmann::json ToJson(Modio::Detail::DynamicBuffer ResponseBuffer)
		{
			Modio::Detail::Buffer LinearBuffer(ResponseBuffer.size());
			Modio::Detail::BufferCopy(LinearBuffer, ResponseBuffer);

			return nlohmann::json::parse(LinearBuffer.Data());
		}

		inline nlohmann::json ToJson(Modio::filesystem::path Path)
		{
			return nlohmann::json::parse(Path.string());
		}

		template<typename T>
		inline T MarshalResponse(Modio::Detail::DynamicBuffer ResponseBuffer)
		{
			// @todonow: I have managed to get in results of size 1 here that crash from ListAllMods
			T ResultStructure;
			using nlohmann::from_json;
			from_json(ToJson(ResponseBuffer), ResultStructure);

			return ResultStructure;
		}

		template<typename T>
		inline T MarshalSubobjectResponse(std::string SubobjectName, Modio::Detail::DynamicBuffer ResponseBuffer)
		{
			T ResultStructure;
			using nlohmann::from_json;

			nlohmann::json Json = ToJson(ResponseBuffer);
			nlohmann::json SubobjectJson;
			if (GetSubobjectSafe(Json, SubobjectName, SubobjectJson))
			{
				from_json(SubobjectJson, ResultStructure);
			}

			return ResultStructure;
		}

	} // namespace Detail
} // namespace Modio
