#pragma once
#include "ModioGeneratedVariables.h"
#include "cache/CacheServiceImplementation.h"
#include <asio.hpp>
#include <string>
#include <memory>

namespace Modio
{
	namespace Detail
	{
		class CacheService : public asio::detail::service_base<CacheService>
		{
			std::shared_ptr<CacheServiceImplementation> PlatformImplementation;

		public:
			MODIO_IMPL explicit CacheService(asio::io_context& IOService);

			MODIO_IMPL void Shutdown();

			using implementation_type = CacheServiceImplementation::IOObjectImplementationType;

			MODIO_IMPL void construct(implementation_type& Implementation);

			MODIO_IMPL void destroy(implementation_type& Implementation);

			MODIO_IMPL void SetCacheExpireTime(std::chrono::steady_clock::duration ExpireTime);

			MODIO_IMPL void AddToCache(std::string ResourceURL, class Modio::Detail::DynamicBuffer ResponseData);

			MODIO_IMPL Modio::Optional<Modio::Detail::DynamicBuffer> FetchFromCache(std::string ResourceURL);

			MODIO_IMPL void ClearCache();
		};
	} // namespace Detail
} // namespace Modio

#ifndef MODIO_SEPARATE_COMPILATION
#include "ModioCacheService.ipp"
#endif