#pragma once

#include "ModioGeneratedVariables.h"
#include "modio/core/ModioBuffer.h"
#include "modio/http/ModioHttpParams.h"
#include "modio/http/ModioHttpService.h"
#include <asio.hpp>

namespace Modio
{
	namespace Detail
	{
		class HttpRequest : public asio::basic_io_object<HttpService>
		{
			HttpRequestParams RequestParameters;

		public:
			MODIO_IMPL explicit HttpRequest(HttpRequestParams RequestParams);

			MODIO_IMPL explicit HttpRequest(asio::io_context& Context, HttpRequestParams RequestParams);

			MODIO_IMPL HttpRequest(HttpRequest&& Other);
			MODIO_IMPL ~HttpRequest();
			MODIO_IMPL HttpRequestParams& Parameters();

			MODIO_IMPL std::uint32_t GetResponseCode();

			// pass in a const buffer as well?
			template<typename CompletionTokenType>
			auto async_Send(CompletionTokenType&& Token)
			{
				// TODO: @Modio-core Make sure request hasn't been sent already
				get_service().async_SendRequest(get_implementation(),
												std::forward<CompletionTokenType>(std::move(Token)));
			}

			template<typename CompletionTokenType>
			auto async_ReadResponseHeaders(CompletionTokenType&& Token)
			{
				// Must have sent the request first before we read headers

				get_service().async_ReadResponseHeaders(get_implementation(),
														std::forward<CompletionTokenType>(std::move(Token)));
			}

			// pass in a mutable buffer to read into as well?
			template<typename CompletionTokenType>
			auto async_ReadSomeFromResponseBody(DynamicBuffer DynamicBufferInstance, CompletionTokenType&& Token)
			{
				get_service().async_ReadSomeFromResponseBody(get_implementation(), DynamicBufferInstance,
															 std::forward<CompletionTokenType>(std::move(Token)));
			}
		};
	} // namespace Detail
}; // namespace Modio

#ifndef MODIO_SEPARATE_COMPILATION
#include "ModioHttpRequest.ipp"
#endif