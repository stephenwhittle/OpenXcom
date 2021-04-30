#ifdef MODIO_SEPARATE_COMPILATION
	#include "modio/cache/ModioCacheService.h"
#endif
namespace Modio
{
	namespace Detail
	{
		CacheService::CacheService(asio::io_context& IOService)
			: asio::detail::service_base<CacheService>(IOService),
			  CacheExpiryTimer(get_io_context())
		{
			CacheInstance = std::make_shared<Cache>();
		}

		void CacheService::Shutdown()
		{
			CacheExpiryTimer.cancel();
			ClearCache();
		}

		void CacheService::construct(implementation_type& Implementation) {}

		void CacheService::destroy(implementation_type& Implementation) {}

		void CacheService::SetCacheExpireTime(std::chrono::steady_clock::duration ExpireTime)
		{
			CacheExpiryTime = ExpireTime;
		}

		void CacheService::AddToCache(std::string ResourceURL, Modio::Detail::DynamicBuffer ResponseData)
		{
			auto Hasher = std::hash<std::string>();
			std::uint32_t URLHash = Hasher(ResourceURL);

			CacheInstance->CacheEntries.insert_or_assign(URLHash, ResponseData);

			CacheExpiryTimer.expires_after(CacheExpiryTime);

			auto DeleteCacheEntry = [WeakCacheReference = std::weak_ptr(CacheInstance), URLHash](std::error_code) {
				std::shared_ptr<Cache> CacheReference = WeakCacheReference.lock();
				if (CacheReference)
				{
					CacheReference->CacheEntries.erase(URLHash);
				}
			};

			CacheExpiryTimer.async_wait(DeleteCacheEntry);
		}

		Modio::Optional<Modio::Detail::DynamicBuffer> CacheService::FetchFromCache(std::string ResourceURL)
		{
			auto Hasher = std::hash<std::string>();
			std::uint32_t URLHash = Hasher(ResourceURL);

			auto CacheEntryIterator = CacheInstance->CacheEntries.find(URLHash);
			if (CacheEntryIterator != CacheInstance->CacheEntries.end())
			{
				return (CacheEntryIterator)->second;
			}
			else
			{
				return {};
			}
		}

		void CacheService::ClearCache()
		{
			CacheInstance.reset(new Cache());
		}
	} // namespace Detail
} // namespace Modio
