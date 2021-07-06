#pragma once

#include "ModioGeneratedVariables.h"
#include "http/HttpImplementation.h"
#include "modio/detail/ModioOperationQueue.h"
#include "modio/detail/AsioWrapper.h"
#include <iostream>
#include <memory>

namespace Modio
{
	namespace Detail
	{
		class HttpService : public asio::detail::service_base<HttpService>
		{
			std::shared_ptr<HttpImplementation> PlatformImplementation;
			// Using shared_ptr here because queue tickets observe the queue
			std::shared_ptr<Modio::Detail::OperationQueue> APIQueue;
			std::shared_ptr<Modio::Detail::OperationQueue> FileDownloadQueue;

		public:
			MODIO_IMPL explicit HttpService(asio::io_context& IOService);

			/// <summary>
			/// The type of the underlying platform implementation of the HTTP IO object
			/// </summary>
			using implementation_type = HttpImplementation::IOObjectImplementationType;

			MODIO_IMPL void construct(implementation_type& Implementation);

			MODIO_IMPL void move_construct(implementation_type& Implementation, implementation_type& Other);

			MODIO_IMPL void move_assign(implementation_type& Implementation, Modio::Detail::HttpService& OtherService,
							 implementation_type& Other);

			MODIO_IMPL void converting_move_construct(implementation_type& impl, Modio::Detail::HttpService&,
										   implementation_type& other_impl);

			MODIO_IMPL void converting_move_assign(implementation_type& impl, Modio::Detail::HttpService& other_service,
										implementation_type& other_impl);

			MODIO_IMPL void destroy(implementation_type& Implementation);

			template<typename CompletionHandler>
			auto InitializeAsync(CompletionHandler Handler)
			{
				return PlatformImplementation->InitializeHTTPAsync(
					std::forward<CompletionHandler>(std::move(Handler)));
			}

			template<typename CompletionHandler>
			auto SendRequestAsync(implementation_type& PlatformIOObject, CompletionHandler Handler)
			{
				return PlatformImplementation->SendRequestAsync(PlatformIOObject,
																 std::forward<CompletionHandler>(std::move(Handler)));
			}

			template<typename CompletionHandler>
			auto ReadResponseHeadersAsync(implementation_type& PlatformIOObject, CompletionHandler Handler)
			{
				return PlatformImplementation->ReadResponseHeadersAsync(
					PlatformIOObject, std::forward<CompletionHandler>(std::move(Handler)));
			}

			template<typename CompletionHandler>
			auto ReadSomeFromResponseBodyAsync(implementation_type& PlatformIOObject,
												DynamicBuffer DynamicBufferInstance, CompletionHandler&& Handler)
			{
				return PlatformImplementation->ReadSomeFromResponseBodyAsync(
					PlatformIOObject, DynamicBufferInstance, std::forward<CompletionHandler>(std::move(Handler)));
			}

			MODIO_IMPL Modio::Detail::OperationQueue::Ticket GetAPIRequestTicket();

			MODIO_IMPL Modio::Detail::OperationQueue::Ticket GetFileDownloadTicket();

			MODIO_IMPL void Shutdown();

		private:
			MODIO_IMPL void shutdown_service();
			// thread?
		};
	} // namespace Detail

} // namespace Modio

#ifndef MODIO_SEPARATE_COMPILATION
#include "ModioHttpService.ipp"
#endif