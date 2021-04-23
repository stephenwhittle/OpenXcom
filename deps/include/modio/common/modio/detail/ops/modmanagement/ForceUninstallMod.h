#pragma once
#include "modio/detail/ops/modmanagement/UninstallMod.h"
#include "modio/detail/ops/SaveModCollectionToStorage.h"
#include <asio.hpp>

#include <asio/yield.hpp>
namespace Modio
{
	namespace Detail
	{
		class ForceUninstallMod
		{
		public:
			ForceUninstallMod(Modio::ModID ModToRemove) : ModToRemove(ModToRemove) {};
			
			template<typename CoroType>
			void operator()(CoroType& Self, Modio::ErrorCode ec = {})
			{
				reenter(CoroutineState) {
					yield Modio::Detail::async_UninstallMod(ModToRemove, std::move(Self));
					if (ec)
					{
						Self.complete(ec);
						return;
					}
					else
					{
						yield Modio::Detail::SaveModCollectionToStorageAsync(std::move(Self));

						Self.complete({});
						return;
					}
				}
			}

		private:
			Modio::ModID ModToRemove;
			asio::coroutine CoroutineState;
		};
	}
}
#include <asio/unyield.hpp>