#pragma once
#include "modio/http/ModioHttpParams.h"

namespace Modio
{
	namespace Detail
	{
		class IHttpRequestImplementation
		{
		public:
			virtual Modio::Detail::HttpRequestParams& GetParameters() = 0;
			virtual std::uint32_t GetResponseCode() = 0;
		};
	} // namespace Detail
} // namespace Modio