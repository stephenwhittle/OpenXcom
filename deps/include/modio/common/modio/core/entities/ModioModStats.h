#pragma once

#include <string>

namespace Modio
{

	/// @docpublic
	/// @brief Contains download stats and ratings for a mod
	struct ModStats
	{		
		/// @brief Current rank of the mod.
		std::int64_t PopularityRankPosition;
		/// @brief Number of ranking spots the current rank is measured against. 
		std::int64_t PopularityRankTotalMods;
		/// @brief Number of total mod downloads. 
		std::int64_t DownloadsTotal;
		/// @brief Number of total users who have subscribed to the mod. 
		std::int64_t SubscribersTotal;
		/// @brief Number of times this mod has been rated.
		std::int64_t RatingTotal;
		/// @brief Number of positive ratings.
		std::int64_t RatingPositive;
		/// @brief Number of negative ratings.
		std::int64_t RatingNegative;
		/// @brief Number of positive ratings, divided by the total ratings to determine it’s percentage score.
		std::int64_t RatingPercentagePositive;
		/// @brief Overall rating of this item calculated using the [Wilson score confidence
		/// interval](https://www.evanmiller.org/how-not-to-sort-by-average-Ratinghtml). This column is good to sort
		/// on, as it will order items based on number of ratings and will place items with many positive ratings
		/// above those with a higher score but fewer ratings.
		double RatingWeightedAggregate;
		/// @brief Textual representation of the rating in format: Overwhelmingly
		/// Positive -> Very Positive -> Positive -> Mostly Positive -> Mixed ->
		/// Negative -> Mostly Negative -> Very Negative -> Overwhelmingly
		/// Negative -> Unrated
		std::string RatingDisplayText;
	};

	static void from_json(const nlohmann::json& Json, Modio::ModStats& ModStats)
	{
		Detail::ParseSafe(Json, ModStats.PopularityRankPosition, "popularity_rank_position");
		Detail::ParseSafe(Json, ModStats.PopularityRankTotalMods, "popularity_rank_total_mods");
		Detail::ParseSafe(Json, ModStats.DownloadsTotal, "downloads_total");
		Detail::ParseSafe(Json, ModStats.SubscribersTotal, "subscribers_total");
		Detail::ParseSafe(Json, ModStats.RatingTotal, "ratings_total");
		Detail::ParseSafe(Json, ModStats.RatingPositive, "ratings_positive");
		Detail::ParseSafe(Json, ModStats.RatingNegative, "ratings_negative");
		Detail::ParseSafe(Json, ModStats.RatingPercentagePositive, "ratings_percentage_positive");
		Detail::ParseSafe(Json, ModStats.RatingWeightedAggregate, "ratings_weighted_aggregate");
		Detail::ParseSafe(Json, ModStats.RatingDisplayText, "ratings_display_text");
	}

	static void to_json(nlohmann::json& Json, const Modio::ModStats Stats)
	{
		Json = nlohmann::json {{"popularity_rank_position", Stats.PopularityRankPosition},
							   {"popularity_rank_total_mods", Stats.PopularityRankTotalMods},
							   {"downloads_total", Stats.DownloadsTotal},
							   {"subscribers_total", Stats.SubscribersTotal},
							   {"ratings_total", Stats.RatingTotal},
							   {"ratings_positive", Stats.RatingPositive},
							   {"ratings_negative", Stats.RatingNegative},
							   {"ratings_percentage_positive", Stats.RatingPercentagePositive},
							   {"ratings_weighted_aggregate", Stats.RatingWeightedAggregate},
							   {"ratings_display_text", Stats.RatingDisplayText}};
	}
} // namespace Modio