#pragma once

namespace Modio
{
	namespace Detail
	{
		class IHttpRequestImplementation
		{
			public:
			virtual HttpRequestParams& GetParameters() = 0;
			virtual std::uint32_t GetResponseCode() = 0;
		};
	}
}