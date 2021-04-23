#pragma once
#include "modio/core/ModioStdTypes.h"
#include <asio.hpp>

#include <asio/yield.hpp>
namespace Modio
{
	namespace Detail
	{
		/// @brief Internal operation used by AsyncShutdown to run a single outstanding handler on the IO context or
		/// else time out
		class AsyncShutdownRunOne
		{
		public:
			AsyncShutdownRunOne(std::shared_ptr<asio::io_context> InTargetContext) : TargetContext(InTargetContext){};

			template<typename CoroType>
			void operator()(CoroType& Self, Modio::ErrorCode ec = {})
			{
				TargetContext->run_one_for(std::chrono::milliseconds(1));
				Self.complete(Modio::ErrorCode {});
			}
			private:
				std::shared_ptr<asio::io_context> TargetContext;
		};
		template<typename CompletionHandlerType>
		void ShutdownRunOneAsync(std::shared_ptr<asio::io_context> OldContext,
								 CompletionHandlerType&& OnOneHandlerCalled)
		{
			return asio::async_compose<CompletionHandlerType, void(Modio::ErrorCode)>(AsyncShutdownRunOne(OldContext),
				OnOneHandlerCalled, Modio::Detail::Services::GetGlobalContext().get_executor());
		}

		class AsyncShutdown
		{
		public:
			AsyncShutdown(std::shared_ptr<asio::io_context> ContextToFlush) : ContextToFlush(ContextToFlush)
			{
				ContextToFlush->restart();
			}

			template<typename CoroType>
			void operator()(CoroType& Self, Modio::ErrorCode ec = {})
			{
				reenter(CoroutineState)
				{
					while (!ContextToFlush->stopped())
					{
						ContextToFlush->run_one_for(std::chrono::milliseconds(1));
						// If we use the op, then we get stack overflow if the shutdown takes a long time
						//yield ShutdownRunOneAsync(ContextToFlush, std::move(Self));
					}
					Self.complete(Modio::ErrorCode {});
				}
			}

		private:
			asio::coroutine CoroutineState;
			std::shared_ptr<asio::io_context> ContextToFlush;
		};
	} // namespace Detail
} // namespace Modio
#include <asio/unyield.hpp>