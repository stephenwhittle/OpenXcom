#pragma once
#include "modio/core/ModioBuffer.h"
#include <asio/io_service.hpp>
#include <asio/steady_timer.hpp>
#include <memory>
#include <unordered_map>

namespace Modio::Detail
{
	class CacheServiceImplementation : public std::enable_shared_from_this<CacheServiceImplementation>
	{
		asio::io_context::service& OwningService;

		struct Cache
		{
			std::unordered_map<std::uint32_t, Modio::Detail::DynamicBuffer> CacheEntries;
		};

		std::shared_ptr<Cache> CacheInstance;
		asio::steady_timer CacheExpiryTimer;
		std::chrono::steady_clock::duration CacheExpiryTime = std::chrono::seconds(15);

	public:
		using IOObjectImplementationType = std::uint32_t;

		CacheServiceImplementation(asio::io_context::service& OwningService)
			: OwningService(OwningService),
			  CacheExpiryTimer(OwningService.get_io_context())
		{
			CacheInstance = std::make_shared<Cache>();
		}

		void SetCacheExpireTime(std::chrono::steady_clock::duration ExpireTime)
		{
			CacheExpiryTime = ExpireTime;
		}

		void Shutdown()
		{
			CacheExpiryTimer.cancel();
		}

		/// <summary>
		/// Stub method for initializing an implementation object
		/// Cache services don't require one so this is a no-op
		/// </summary>
		/// <param name=""></param>
		void InitializeIOObjectImplementation(IOObjectImplementationType&) {}

		void AddToCache(std::string ResourceURL, Modio::Detail::DynamicBuffer ResponseBuffer)
		{
			auto Hasher = std::hash<std::string>();
			std::uint32_t URLHash = Hasher(ResourceURL);

			CacheInstance->CacheEntries.insert_or_assign(URLHash, ResponseBuffer);

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

		Modio::Optional<Modio::Detail::DynamicBuffer> FetchFromCache(std::string ResourceURL)
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

		void ClearCache()
		{
			CacheInstance.reset(new Cache());
		}
	};
} // namespace Modio::Detail