#pragma once

#include "modio/detail/ModioJsonHelpers.h"
#include <cstdint>
#include <nlohmann/json.hpp>

namespace Modio
{
	/// <summary>
	/// Base class for all results that returns paged results
	/// </summary>
	class PagedResult
	{
		friend static void from_json(const nlohmann::json& Json, Modio::PagedResult& PagedResult);

		std::int32_t PageIndex;
		std::int32_t PageSize;
		std::int32_t PageCount;
		std::int32_t TotalResultCount; // @todo: Reiterate this variable name
		std::int32_t ResultCount; // @todo: Reiterate this variable name
	public:
		inline std::int32_t GetPageCount() const
		{
			return PageCount;
		}
		
		inline std::int32_t GetPageIndex() const
		{
			return PageIndex;
		}
		inline std::int32_t GetPageSize() const
		{
			return PageSize;
		}

		inline std::int32_t GetTotalResultCount() const
		{
			return TotalResultCount;
		}

		inline std::int32_t GetResultCount() const
		{
			return ResultCount;
		}
	};

	void from_json(const nlohmann::json& Json, Modio::PagedResult& PagedResult)
	{
		Detail::ParseSafe(Json, PagedResult.ResultCount, "result_count");
		Detail::ParseSafe(Json, PagedResult.PageSize, "result_limit");
		Detail::ParseSafe(Json, PagedResult.TotalResultCount, "result_total");

		// Convert offset to pages
		int ResultOffset = 0;
		Detail::ParseSafe(Json, ResultOffset, "result_offset");
		PagedResult.PageIndex = std::floor(ResultOffset / (float) PagedResult.PageSize);
		PagedResult.PageCount = std::ceil(PagedResult.TotalResultCount / (float) PagedResult.PageSize);
	}
} // namespace Modio