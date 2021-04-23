#pragma once

#include "modio/core/ModioLogEnum.h"
#include "modio/core/ModioLogService.h"
#include "modio/core/ModioServices.h"
#include <asio.hpp>

namespace Modio
{
	namespace Detail
	{
		/// <summary>
		/// Generic threadsafe logging object. Can either construct and retain an instance or log via a temporary
		/// </summary>
		class Logger : public asio::basic_io_object<Modio::Detail::LogService>
		{
		protected:
			template<typename... ArgTypes>
			void LogImmediate(LogLevel Level, LogCategory Category, std::string Format, ArgTypes... Args)
			{
				get_service().LogImmediate(get_implementation(), Level, Category, Format, Args...);
			}

		public:
			/// <summary>
			/// Explicit constructor for a Logger that posts its messages via the specified io_context
			/// </summary>
			/// <param name="Context"></param>
			explicit Logger(asio::io_context& Context) : asio::basic_io_object<Modio::Detail::LogService>(Context) {}

			/// <summary>
			/// Convenience constructor for a Logger that posts log messages via the global io_context
			/// </summary>
			explicit Logger()
				: asio::basic_io_object<Modio::Detail::LogService>(Modio::Detail::Services::GetGlobalContext())
			{}

			/// <summary>
			/// Print a message to the platform output device
			/// </summary>
			/// <param name="Level">Modio::LogLevel indicating the severity of the message</param>
			/// <param name="Category">Modio::LogCategory indicating which service the message is associated
			/// with</param> <param name="Format">Format string for the log output. Uses fmtlib syntax for
			/// substitutions</param> <param name="...Args">Any arguments to substitute into the format string</param>
			template<typename... ArgTypes>
			void Log(LogLevel Level, LogCategory Category, std::string Format, ArgTypes... Args)
			{
				get_service().LogImmediate(get_implementation(), Level, Category, Format, Args...);
			}
		};
		class ScopedLogger : Logger
		{
			std::string ScopeName;
			LogCategory Category;

		public:
			explicit ScopedLogger(LogCategory Category, std::string ScopeName)
				: ScopeName(ScopeName),
				  Category(Category)
			{
				LogImmediate(LogLevel::Info, Category, "Entered {}", ScopeName);
			}
			~ScopedLogger()
			{
				LogImmediate(LogLevel::Info, Category, "Left {}", ScopeName);
			}
		};
	} // namespace Detail
} // namespace Modio