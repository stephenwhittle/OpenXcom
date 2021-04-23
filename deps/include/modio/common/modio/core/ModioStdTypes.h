#pragma once
#include "modio/core/ModioFilesystem.h"
#include "tl/optional.hpp"
#include <memory>
#include <nlohmann/json.hpp>
#include <system_error>

#include <cassert>
#include <chrono>
#include <cstdint>
#include <string>

namespace asio
{
	class mutable_buffer;
	class const_buffer;
} // namespace asio

namespace Modio
{
	/// @docpublic
	/// @brief Enum representing what environment your game is deployed in
	enum class Environment
	{
		Test,
		Live
	};

	/// @docpublic
	/// @brief Enum representing mod logo sizes
	enum class LogoSize : std::uint8_t
	{
		Original,
		Thumb320, ///< 320x180
		Thumb640, ///< 640x360
		Thumb1280 ///< 1280x720
	};

	/// @docpublic
	/// @brief Enum representing avatar image sizes
	enum class AvatarSize : std::uint8_t
	{
		Original,
		Thumb50, ///< 50x50
		Thumb100 ///< 100x100
	};

	enum class GallerySize : std::uint8_t
	{
		Original,
		Thumb320 ///< 320x180
	};

	template<typename Rep, typename Period = std::ratio<1>>
	using Duration = std::chrono::duration<Rep, Period>;

	using ErrorCode = std::error_code;

	/// @docpublic
	/// @brief nullable wrapper around object of type T. Used by async functions that return values - empty on function
	/// error/failure
	/// @tparam T
	template<typename T>
	using Optional = tl::optional<T>;

	using MutableBufferView = asio::mutable_buffer;
	using ConstBufferView = asio::const_buffer;

	template<typename Object>
	using StableStorage = std::shared_ptr<Object>;

	template<typename Object, class... _Types>
	StableStorage<Object> MakeStable(_Types&&... _Args)
	{
		return std::make_shared<Object>(std::forward<_Types>(_Args)...);
	}

	// using UserID = std::uint64_t;
	// using ModID = std::uint64_t;
	// using CommentID = std::uint64_t;
	// using MediaFileID = std::uint64_t;
	using Timestamp = std::uint64_t;
	using GalleryIndex = int;

	// TODO: @Modio-core strong type for filesystem::paths that are directories?

	/// @docpublic
	/// @brief Strong integer type to prevent accidental function parameter reordering and prevent unwanted implicit
	/// conversions
	/// @tparam UnderlyingIntegerType The underlying integer type the StrongInteger represents
	template<typename UnderlyingIntegerType>
	struct StrongInteger
	{
	private:
		UnderlyingIntegerType Value = 0;

	public:
		// Force explicit use of the constructor when we need a StrongInteger (ie this means you must construct a
		// StrongInteger to pass into a function)

		/// @docpublic
		/// @brief explicit constructor from a value of the underlying integer type
		/// @param Value initial value
		constexpr explicit StrongInteger(UnderlyingIntegerType Value) : Value(Value) {};

		constexpr explicit StrongInteger() = default;

		/// @brief Allow implicit conversion to the underlying integer type
		constexpr operator UnderlyingIntegerType() const
		{
			return Value;
		}

		/// @brief Allow us to explicit convert to our underlying type
		constexpr UnderlyingIntegerType operator*() const
		{
			return Value;
		}

		///@{
		/** Arithmetic and logical operator overloads */

		constexpr StrongInteger& operator+=(const StrongInteger Other)
		{
			Value += Other.Value;
			return *this;
		}

		constexpr StrongInteger& operator-=(const StrongInteger Other)
		{
			Value -= Other.Value;
			return *this;
		}

		constexpr StrongInteger operator+(const StrongInteger Other) const
		{
			return StrongInteger(Value + Other.Value);
		}

		constexpr StrongInteger operator-(const StrongInteger Other) const
		{
			return StrongInteger(Value - Other.Value);
		}

		constexpr bool operator>(const StrongInteger Other) const
		{
			return Value > Other.Value;
		}

		constexpr bool operator<(const StrongInteger Other) const
		{
			return Value < Other.Value;
		}
		constexpr bool operator>=(const StrongInteger Other) const
		{
			return Value >= Other.Value;
		}

		constexpr bool operator<=(const StrongInteger Other) const
		{
			return Value <= Other.Value;
		}

		constexpr bool operator==(const StrongInteger Other) const
		{
			return Value == Other.Value;
		}

		constexpr bool operator!=(const StrongInteger Other) const
		{
			return Value != Other.Value;
		}
		///@}
		friend void from_json(const nlohmann::json& Json, Modio::StrongInteger<UnderlyingIntegerType>& Integer)
		{
			using nlohmann::from_json;
			UnderlyingIntegerType RawValue;
			from_json(Json, RawValue);
			Integer = Modio::StrongInteger<UnderlyingIntegerType>(RawValue);
		}

		friend void to_json(nlohmann::json& Json, const Modio::StrongInteger<UnderlyingIntegerType>& Integer)
		{
			using nlohmann::to_json;
			UnderlyingIntegerType RawValue = Integer;
			to_json(Json, RawValue);
		}
	};

	/// @docpublic
	/// @brief Strong type representing an email authentication code sent to a user email address
	struct EmailAuthCode
	{
		std::string InternalCode;

		/// @brief Explicit constructor
		/// @param Code The code sent to the user's email
		explicit EmailAuthCode(const std::string& Code) : InternalCode(Code) {}
	};

	/// @docpublic
	/// @brief Strong type representing an email address
	struct EmailAddress
	{
		std::string InternalAddress;

		/// @brief Explicit constructor
		/// @param Code The code sent to the user's email
		explicit EmailAddress(const std::string& Code) : InternalAddress(Code) {}
	};

	/// @docpublic
	/// @brief Strong type representing an api key
	struct ApiKey
	{
		std::string InternalApiKey;

		ApiKey() = default;

		/// @brief Explicit constructor
		/// @param Code The code sent to the user's email
		explicit ApiKey(const std::string& InApiKey) : InternalApiKey(InApiKey) {}
	};

	// Needs to be subclassed rather than type aliased so we don't accidentally provide the wrong type to a function

	/// @brief Strong type for User IDs
	struct UserID : public StrongInteger<std::int64_t>
	{
		using StrongInteger::StrongInteger;
	};

	/// @docpublic
	/// @brief Strong type for Mod IDs
	struct ModID : public StrongInteger<std::int64_t>
	{
		using StrongInteger::StrongInteger;
	};

	/// @docpublic
	/// @brief Strong type for File Metadata IDs
	struct FileMetadataID : public StrongInteger<std::int64_t>
	{
		using StrongInteger::StrongInteger;
	};

	/// @docpublic
	/// @brief Strong type for Comment ID
	struct CommentID : public StrongInteger<std::int64_t>
	{
		using StrongInteger::StrongInteger;
	};

	/// @brief Strong type for Media File ID
	struct MediaFileID : public StrongInteger<std::int64_t>
	{
		using StrongInteger::StrongInteger;
	};

	/// @brief Strong type for file offsets
	struct FileOffset : public StrongInteger<std::uintmax_t>
	{
		using StrongInteger::StrongInteger;
	};

	/// @brief Strong type for file sizes
	struct FileSize : public StrongInteger<std::uintmax_t>
	{
		using StrongInteger::StrongInteger;
	};

	/// @docpublic
	/// @brief Strong type for a game ID
	/// @todonow: Rename to GameID
	struct GameID : public StrongInteger<std::int64_t>
	{
		using StrongInteger::StrongInteger;

	};

	/// @brief Simple struct to encapsulate data passed to external authentication systems
	struct AuthenticationParams
	{
		std::string AuthToken;
		Modio::Optional<std::string> UserEmail;
		bool bUserHasAcceptedTerms = false;
	};

	enum class AuthenticationProvider
	{
		XboxLive,
		Epic,
		Steam,
		GoG,
		Itch,
		Switch,
		Discord
	};
} // namespace Modio
