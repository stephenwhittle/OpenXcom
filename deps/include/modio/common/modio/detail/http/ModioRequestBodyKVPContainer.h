#pragma once

#include <vector>
#include <utility>

namespace Modio::Detail
{
	inline std::string ToString(const std::string& Value)
	{
		return Value;
	}

	inline std::string ToString(std::int64_t Value)
	{
		return std::to_string(Value);
	}

	/// <summary>
	/// Class responsible to marshal the body of a http request. You pass it key value pairs and it stores
	/// them off in a pair so we can set the body for the request
	/// </summary>
	class RequestBodyKVPContainer
	{
		using InternalContainerClass = std::vector<std::pair<std::string, std::string>>;
		InternalContainerClass InternalContainer;

	public:
		template<typename T>
		void Add(const std::string& Key, const T& Value)
		{
			InternalContainer.push_back({Key, ToString(Value)});
		}

		template<typename T>
		void Add(const std::string& Key, const Modio::Optional<T>& Value)
		{
			if (Value)
			{
				InternalContainer.push_back({Key, ToString(Value.value())});
			}
		}

		using iterator = typename InternalContainerClass::iterator;
		using const_iterator = typename InternalContainerClass::const_iterator;
		iterator begin()
		{
			return InternalContainer.begin();
		}

		const_iterator begin() const
		{
			return InternalContainer.begin();
		}

		iterator end()
		{
			return InternalContainer.end();
		}

		const_iterator end() const
		{
			return InternalContainer.end();
		}
	};
}