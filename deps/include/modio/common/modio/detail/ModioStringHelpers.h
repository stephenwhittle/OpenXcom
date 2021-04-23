#pragma once

#include "modio/core/ModioStdTypes.h"
#include <cctype>
#include <string>

namespace Modio
{
	namespace Detail
	{
		namespace String
		{
			/// <summary>
			/// Returns true if the string starts with the specified prefix
			/// </summary>
			/// <param name="String"></param>
			/// <param name="Prefix"></param>
			/// <returns></returns>
			inline bool StartWith(const std::string& String, const std::string& Prefix)
			{
				if (String.length() >= Prefix.length())
				{
					return String.compare(0, Prefix.length(), Prefix) == 0;
				}
				return false;
			}
			

			/// <summary>
			/// URLEncode a string
			/// </summary>
			/// <param name="String"></param>
			/// <returns></returns>
			static std::string URLEncode(const std::string& String)
			{
				std::string Result;
				Result.reserve(String.size() * 3);
				// @todo: Support UTF8
				auto IsValidCharacter = [](char Character) {
					return (Character >= '0' && Character < '9') || (Character >= 'a' && Character < 'z') ||
						   (Character >= 'A' && Character < 'Z') || Character == '-' || Character == '_' ||
						   Character == '.' || Character == '~';
				};

				static const char HexChars[16] = {'0', '1', '2', '3', '4', '5', '6', '7',
												  '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
				for (int Idx = 0; Idx < String.size(); ++Idx)
				{
					const char& Char = String[Idx];
					if (IsValidCharacter(Char))
					{
						Result += Char;
					}
					else
					{
						Result += '%';
						Result += HexChars[(Char & 0xF0) >> 4];
						Result += HexChars[(Char & 0x0F) >> 0];
					}
				}

				return Result;
			}

			/// <summary>
			/// Replaces all occurrences of a string to another
			/// </summary>
			/// <param name="Str">The string we want to replace all occurences in, replace will happen inline</param>
			/// <param name="From">What we want to replace</param>
			/// <param name="To">What we want to replace it with</param>
			static void ReplaceAll(std::string& Str, const std::string& From, const std::string& To)
			{
				size_t StartPos = 0;
				while ((StartPos = Str.find(From, StartPos)) != std::string::npos)
				{
					Str.replace(StartPos, From.length(), To);
					StartPos += To.length(); // Handles case where 'To' is a substring of 'From'
				}
			}

			/// <summary>
			/// Splits a string based on a string delimiter
			/// </summary>
			/// <param name="Str">String to split</param>
			/// <param name="Delimiter">Delimiter to split on</param>
			/// <returns>Array with strings</returns>
			static std::vector<std::string> Split(const std::string& Str, const std::string& Delimiter)
			{
				std::vector<std::string> Result;
				size_t OldLoc = 0;
				size_t StartPos = 0;
				while ((StartPos = Str.find(Delimiter, StartPos)) != std::string::npos)
				{
					Result.push_back(Str.substr(OldLoc, StartPos - OldLoc));
					StartPos += Delimiter.length();
					OldLoc = StartPos;
				}
				// in string "cheese beaver donkey", we won't have consumed the donkey here
				// Ensure that we have any more characters to consume
				if (Str.length() - OldLoc > 0)
				{
					Result.push_back(Str.substr(OldLoc, Str.length() - OldLoc));
				}

				return Result;
			}

			/// <summary>
			/// Case sensitive comparison that checks if a string ends with another string
			/// </summary>
			/// <param name="FullString"></param>
			/// <param name="Ending"></param>
			/// <returns></returns>
			static bool EndsWith(const std::string& FullString, const std::string& Ending)
			{
				if (FullString.length() >= Ending.length())
				{
					return FullString.compare(FullString.length() - Ending.length(), Ending.length(), Ending) == 0;
				}
				return false;
			}

			/// <summary>
			/// Get the filename of a URL (eg, http://mod.io/index.html returns index.html)
			/// </summary>
			/// <param name="URL">Must contain a</param>
			/// <returns></returns>
			static Modio::Optional<std::string> GetFilenameFromURL(const std::string& URL)
			{
				std::size_t LastPos = URL.find_last_of('/');

				// There was no / in the URL, so can't find out where the filename is
				if (LastPos == std::string::npos)
				{
					return {};
				}

				return URL.substr(LastPos + 1);
			}
		} // namespace String
	} // namespace Detail
} // namespace Modio
