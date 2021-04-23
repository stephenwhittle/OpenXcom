#pragma once
#include "ModioGeneratedVariables.h"
#include <asio.hpp>
#include <memory>
#include "modio/detail/compression/zip/CompressionImplementation.h"

namespace Modio
{
	namespace Detail
	{
		class CompressionService : public asio::detail::service_base<CompressionService>
		{
			// TODO: @Modio-Core Better name for Compression/Compressor Implementation?
			std::shared_ptr<CompressionImplementation> PlatformImplementation;

		public:
			explicit CompressionService(asio::io_context& IOService)
				: asio::detail::service_base<CompressionService>(IOService)
			{
				auto NewImplementation = std::make_shared<CompressionImplementation>();
				PlatformImplementation.swap(NewImplementation);
			}

			~CompressionService() {}

			using implementation_type = CompressionImplementation::IOObjectImplementationType;

			MODIO_IMPL void construct(implementation_type& Implementation);

			MODIO_IMPL void destroy(implementation_type& Implementation) {}

			MODIO_IMPL void move_construct(implementation_type& Implementation, implementation_type& Other);

			MODIO_IMPL void move_assign(implementation_type& Implementation,
										Modio::Detail::CompressionService& OtherService,
							 implementation_type& Other);

			MODIO_IMPL void converting_move_construct(implementation_type& impl, Modio::Detail::CompressionService&,
										   implementation_type& other_impl);

			MODIO_IMPL void converting_move_assign(implementation_type& impl,
												   Modio::Detail::CompressionService& other_service,
										implementation_type& other_impl);


		private:
			// TODO: @Modio-core should services make their shutdown_service method private?
			void shutdown_service() {}
		};
	}

} // namespace Modio

#ifndef MODIO_SEPARATE_COMPILATION
#include "ModioCompressionService.ipp"
#endif