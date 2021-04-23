#pragma once

#include "modio/core/ModioServices.h"
#include "logging/LoggerImplementation.h"
#include "modio/core/ModioLogBuffer.h"
#include "modio/core/ModioLogEnum.h"
#include <asio.hpp>

namespace Modio
{
	namespace Detail
	{
		class LogService : public asio::detail::service_base<LogService>
		{
			struct LogMessage
			{
				Modio::LogLevel Level;
				std::string Message;
			};

		public:
			explicit LogService(asio::io_context& IOService)
				: asio::detail::service_base<LogService>(IOService),
				  CurrentLogLevel(LogLevel::Trace),
				  LogStrand(asio::make_strand(IOService))
			{}

			using implementation_type = std::shared_ptr<Modio::Detail::LoggerImplementation>;

			void construct(implementation_type& Implementation)
			{
				Implementation = std::make_shared<LoggerImplementation>(LogStrand);
			}
			void destroy(implementation_type& Implementation)
			{
				Implementation.reset();
			}

			void Shutdown()
			{
				FlushLogBuffer();
			}

			void SetLogLevel(LogLevel Level)
			{
				CurrentLogLevel = Level;
			}

			LogLevel GetLogLevel() const
			{
				return CurrentLogLevel;
			}

			void FlushLogBuffer()
			{
				if (UserCallbackFunction)
				{
					for (const LogMessage& Log : LogBuffer)
					{
						UserCallbackFunction(Log.Level, Log.Message);
					}
				}
				LogBuffer.clear();
			}

			template<typename... ArgTypes>
			void Log(implementation_type& PlatformLoggerObject, LogLevel Level, LogCategory Category,
					 std::string Format, ArgTypes... Args)
			{
				if (Level >= CurrentLogLevel)
				{
					std::string LogOutput = PlatformLoggerObject->Log(Level, Category, Format, Args...);
					LogBuffer.push_back({Level, LogOutput});
				}
			}

			template<typename... ArgTypes>
			void LogImmediate(implementation_type& PlatformLoggerObject, LogLevel Level, LogCategory Category,
							  std::string Format, ArgTypes... Args)
			{
				if (Level >= CurrentLogLevel)
				{
					std::string LogOutput = PlatformLoggerObject->LogImmediate(Level, Category, Format, Args...);
					LogBuffer.push_back({Level, LogOutput});
				}
			}
			// TODO: @modio-core this should not be a static function
			static void SetLogCallback(std::function<void(Modio::LogLevel, const std::string&)> LogCallback)
			{
				// Flush out the current logs
				if (UserCallbackFunction)
				{
					// @todo: What will happen here if there is no service alive as the global context is dead?
					Modio::Detail::Services::GetGlobalService<Modio::Detail::LogService>().FlushLogBuffer();
				}
				UserCallbackFunction = LogCallback;
			}

			/// <summary>
			/// Static function to allow easy setting of the global logging level
			/// </summary>
			/// <param name="Level">The minimum severity level to display</param>
			static void SetGlobalLogLevel(Modio::LogLevel Level)
			{
				auto& LogService = asio::use_service<Modio::Detail::LogService>(Modio::Detail::Services::GetGlobalContext());
				LogService.SetLogLevel(Level);
			}

		private:
			// Todo: @modio-core move this to the SDKSessionData so it's permanently valid
			// Not stored in global state, as it's created during startup, and we want to be able to capture logs from
			// the startup
			static inline std::function<void(Modio::LogLevel, const std::string&)> UserCallbackFunction;

			LogLevel CurrentLogLevel;
			asio::strand<asio::io_context::executor_type> LogStrand;
			std::vector<LogMessage> LogBuffer;
		};
	} // namespace Detail
} // namespace Modio
