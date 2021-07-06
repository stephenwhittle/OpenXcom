#pragma once
#include "ModioGeneratedVariables.h"
#include <cstdint>
#include <nlohmann/json.hpp>

namespace Modio
{
	/// <summary>
	/// Base class for all results that returns paged results
	/// </summary>
	class PagedResult
	{
		

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

		friend MODIO_IMPL void from_json(const nlohmann::json& Json, Modio::PagedResult& PagedResult);
	};

	
} // namespace Modio


#ifndef MODIO_SEPARATE_COMPILATION
	#include "ModioPagedResult.ipp"
#endif
