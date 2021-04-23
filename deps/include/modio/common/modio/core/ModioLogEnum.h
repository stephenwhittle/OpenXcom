#pragma once
namespace Modio
{
	/// @docpublic
	/// @brief Degree of severity for the log output
	enum class LogLevel
	{
		Trace = 0, ///< Detailed low-level debugging output. Not intended for general use
		Info = 1, ///< Informational output containing status messages
		Warning = 2, ///< Warnings about incorrect SDK usage, timeouts
		Error = 3
	};

	/// <summary>
	/// Converts a Modio::LogLevel enum to string for use in logs
	/// </summary>
	/// <param name="Level"></param>
	/// <returns></returns>
	constexpr const char* LogLevelToString(LogLevel Level)
	{
		switch (Level)
		{
			case LogLevel::Trace:
				return "Trace";
			case LogLevel::Info:
				return "Info";
			case LogLevel::Warning:
				return "Warning";
			case LogLevel::Error:
				return "Error";
			default:
				return "UNKNOWN";
		}
	}

	/// @docpublic
	/// @brief The category of the log message (usually the internal service the message is originating from)
	enum class LogCategory
	{
		Core,
		File,
		Http,
		Compression,
		User,
		ModManagement
	};

	/// <summary>
	/// Converts a Modio::LogCategory enum to string for use in logs
	/// </summary>
	/// <param name="Category"></param>
	/// <returns></returns>
	constexpr const char* LogCategoryToString(LogCategory Category)
	{
		switch (Category)
		{
			case LogCategory::Core:
				return "Core";
			case LogCategory::File:
				return "File";
			case LogCategory::Http:
				return "Http";
			case LogCategory::Compression:
				return "Compression";
			case LogCategory::User:
				return "User";
			case LogCategory::ModManagement:
				return "ModManagement";
			default:
				return "UNKNKOWN";
		}
	}
} // namespace Modio
