#pragma once
#include "modio/core/ModioLogEnum.h"
#include <asio.hpp>

namespace Modio
{
	namespace Detail
	{

	/// @internal
	/// @brief Class intended to contain static data and accessors for the IO services the SDK uses internally
	/// May eventually contain all IO service instances
		class Services
		{
		public:
			/// <summary>
			/// Singleton containing the global IO context for all mod.io SDK operations
			/// </summary>
			/// <returns></returns>
			static asio::io_context& GetGlobalContext()
			{
				return *(GetGlobalContextInternal().get());
			}
			static std::shared_ptr<asio::io_context> ResetGlobalContext()
			{
				// Make a copy of the old context. Note this is not a reference, but an actual copy so lifetime is
				// extended.
				std::shared_ptr<asio::io_context> OldContext = GetGlobalContextInternal();
				GetGlobalContextInternal().reset();
				GetGlobalContextInternal() = std::make_shared<asio::io_context>(1);
				return OldContext;
			}
			template<typename ServiceType>
			static ServiceType& GetGlobalService()
			{
				return asio::use_service<ServiceType>(GetGlobalContext());
			}
		private:
			static std::shared_ptr<asio::io_context>& GetGlobalContextInternal()
			{
				struct ContextHolder
				{
					std::shared_ptr<asio::io_context> Context;
					ContextHolder()
					{
						Context = std::make_shared<asio::io_context>(1);
					}
				};
				static ContextHolder InternalContext;
				return InternalContext.Context;
			}
		};
	} // namespace Detail
} // namespace Modio
