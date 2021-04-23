#pragma once

#include "modio/core/ModioStdTypes.h"

#ifdef MODIO_SEPARATE_COMPILATION
	#include "modio/cache/ModioCacheService.h"
#endif
namespace Modio
{
	namespace Detail
	{
		CacheService::CacheService(asio::io_context& IOService)
			: asio::detail::service_base<CacheService>(IOService)
		{
			auto NewImplementation = std::make_shared<CacheServiceImplementation>(*this);
			PlatformImplementation.swap(NewImplementation);
		}

		void CacheService::Shutdown()
		{
			PlatformImplementation->Shutdown();
			ClearCache();
		}

		void CacheService::construct(implementation_type& Implementation)
		{
			PlatformImplementation->InitializeIOObjectImplementation(Implementation);
		}

		void CacheService::destroy(implementation_type& Implementation) {}

		void CacheService::SetCacheExpireTime(std::chrono::steady_clock::duration ExpireTime)
		{
			PlatformImplementation->SetCacheExpireTime(ExpireTime);
		}

		void CacheService::AddToCache(std::string ResourceURL, Modio::Detail::DynamicBuffer ResponseData)
		{
			PlatformImplementation->AddToCache(ResourceURL, ResponseData);
		}

		Modio::Optional<Modio::Detail::DynamicBuffer> CacheService::FetchFromCache(std::string ResourceURL)
		{
			return PlatformImplementation->FetchFromCache(ResourceURL);
		}

		void CacheService::ClearCache()
		{
			PlatformImplementation->ClearCache();
		}
	} // namespace Detail
} // namespace Modio
