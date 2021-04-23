#ifdef MODIO_SEPARATE_COMPILATION
	#include "ModioHttpRequest.h"
#endif

#include "modio/core/ModioLogger.h"

namespace Modio
{
	namespace Detail
	{
		HttpRequest::HttpRequest(HttpRequest&& Other)
			: RequestParameters(std::move(Other.RequestParameters)),
			  asio::basic_io_object<HttpService>(std::move(Other))
		{}

		HttpRequest::HttpRequest(asio::io_context& Context, HttpRequestParams RequestParams)
			: RequestParameters(RequestParams),
			  asio::basic_io_object<HttpService>(Context)
		{
			get_implementation()->Parameters = RequestParams;
			Logger().Log(LogLevel::Trace, LogCategory::Http, "Creating Request for {}",
						 RequestParams.GetFormattedResourcePath());
		}

		HttpRequest::HttpRequest(HttpRequestParams RequestParams)
			: RequestParameters(RequestParams),
			  asio::basic_io_object<HttpService>(Modio::Detail::Services::GetGlobalContext())
		{
			get_implementation()->Parameters = RequestParameters;
			Logger().Log(LogLevel::Trace, LogCategory::Http, "Creating Request for {}",
						 RequestParams.GetFormattedResourcePath());
		}

		HttpRequest::~HttpRequest() {}

		Modio::Detail::HttpRequestParams& HttpRequest::Parameters()
		{
			return get_implementation()->Parameters;
		}

		std::uint32_t HttpRequest::GetResponseCode()
		{
			return get_implementation()->GetResponseCode();
		}

	} // namespace Detail
} // namespace Modio
