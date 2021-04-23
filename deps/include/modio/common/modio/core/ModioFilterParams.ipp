#ifdef MODIO_SEPARATE_COMPILATION
#include "ModioFilterParams.h"
#endif

#include "fmt/chrono.h"

namespace Modio
{
	Modio::FilterParams& FilterParams::SortBy(SortFieldType ByField, SortDirection ByDirection)
	{
		SortField = ByField;
		Direction = ByDirection;
		return *this;
	}

	Modio::FilterParams& FilterParams::NameContains(const std::vector<std::string>& SearchString)
	{
		// @todo: NameContains overloads behave differently. Take single arguments won't append empty values but this
		// version will same seem to be correct of different functions
		if (SearchString.size())
		{
			SearchKeywords.clear();
			SearchKeywords.insert(SearchKeywords.end(), SearchString.begin(), SearchString.end());
		}
		return *this;
	}

Modio::FilterParams& FilterParams::NameContains(std::string SearchString)
	{
		if (SearchString.size())
		{
			SearchKeywords.clear();
			SearchKeywords.push_back(SearchString);
		}
		return *this;
	}

	Modio::FilterParams& FilterParams::MarkedLiveAfter(std::chrono::system_clock::time_point LiveAfter)
	{
		DateRangeBegin = LiveAfter;
		return *this;
	}

	Modio::FilterParams& FilterParams::MarkedLiveBefore(std::chrono::system_clock::time_point LiveBefore)
	{
		DateRangeEnd = LiveBefore;
		return *this;
	}

	Modio::FilterParams& FilterParams::WithTags(std::vector<std::string> NewTags)
	{
		Tags.clear();
		Tags.insert(Tags.end(), NewTags.begin(), NewTags.end());
		return *this;
	}

Modio::FilterParams& FilterParams::WithTags(std::string Tag)
	{
		Tags.clear();
		Tags.push_back(Tag);
		return *this;
	}

	Modio::FilterParams& FilterParams::WithoutTags(std::vector<std::string> NewTags)
	{
		ExcludedTags.clear();
		ExcludedTags.insert(Tags.end(), NewTags.begin(), NewTags.end());
		return *this;
	}

Modio::FilterParams& FilterParams::WithoutTags(std::string Tag)
	{
		ExcludedTags.clear();
		ExcludedTags.push_back(Tag);
		return *this;
	}

	Modio::FilterParams& FilterParams::IndexedResults(std::size_t StartIndex, std::size_t ResultCount)
	{
		IsPaged = false;
		Index = std::max(std::size_t(0), StartIndex);
		Count = std::max(std::size_t(0), ResultCount);
		return *this;
	}

	Modio::FilterParams& FilterParams::PagedResults(std::size_t PageNumber, std::size_t PageSize)
	{
		IsPaged = true;
		Index = std::max(std::size_t(0), PageNumber);
		Count = std::max(std::size_t(0), PageSize);
		return *this;
	}

	FilterParams::FilterParams()
		: SortField(SortFieldType::ID),
		  Direction(SortDirection::Ascending),
		  IsPaged(false),
		  Index(0),
		  Count(100)
	{}

	std::string FilterParams::ToString()
	{
		std::vector<std::string> FilterFields;
		std::string SortStr;
		switch (SortField)
		{
			case SortFieldType::DateMarkedLive:
				SortStr = "date_live";
				break;
			case SortFieldType::DateUpdated:
				SortStr = "date_updated";
				break;
			case SortFieldType::DownloadsToday:
				SortStr = "popular";
				break;
			case SortFieldType::Rating:
				SortStr = "rating";
				break;
			case SortFieldType::SubscriberCount:
				SortStr = "subscriber_count";
				break;
			case SortFieldType::ID:
				SortStr = "id";
				break;
			default:
				break;
		}

		SortStr = fmt::format("_sort={}{}", Direction == SortDirection::Descending ? "-" : "", SortStr);
		FilterFields.push_back(SortStr);

		if (SearchKeywords.size())
		{
			std::string SearchStr;
			for (std::string Keyword : SearchKeywords)
			{
				SearchStr += Keyword + " ";
			}
			SearchStr.resize(SearchStr.size() - 1);
			FilterFields.push_back(fmt::format("_q={}", SearchStr));
		}

		if (DateRangeBegin)
		{
			FilterFields.push_back(fmt::format("date_live-min={}", DateRangeBegin->time_since_epoch()));
		}

		if (DateRangeEnd)
		{
			FilterFields.push_back(fmt::format("date_live-max={}", DateRangeBegin->time_since_epoch()));
		}

		if (Tags.size())
		{
			std::string TagStr;
			for (std::string Tag : Tags)
			{
				TagStr += Tag + " ";
			}
			TagStr.resize(TagStr.size() - 1);
			FilterFields.push_back(fmt::format("tags={}", TagStr));
		}

		if (ExcludedTags.size())
		{
			std::string ExcludedTagStr;
			for (std::string Tag : ExcludedTags)
			{
				ExcludedTagStr += Tag + " ";
			}
			ExcludedTagStr.resize(ExcludedTagStr.size() - 1);
			FilterFields.push_back(fmt::format("tags-not-in={}", ExcludedTagStr));
		}

		std::string ResultLimitStr;
		if (IsPaged)
		{
			FilterFields.push_back(fmt::format("_limit={}&_offset={}", Count, Count * Index));
		}
		else
		{
			FilterFields.push_back(fmt::format("_limit={}&_offset={}", Count, Index));
		}

		std::string FilterString;

		for (std::string Filter : FilterFields)
		{
			FilterString += Filter + "&";
		}
		FilterString.resize(FilterString.size() - 1);
		// FilterString.pop_back();
		return FilterString;
	}

	Modio::FilterParams& FilterParams::AppendValue(std::vector<std::string>& Vector, std::string Tag)
	{
		Tags.push_back(Tag);
		return *this;
	}

}

