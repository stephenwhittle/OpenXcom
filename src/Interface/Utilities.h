#pragma once
#include <string>
#include <vector>
#include "../Engine/Unicode.h"

namespace OpenXcom
{
	class Font;
	class Language;

	struct textInfo
	{
		UString processedText;
		std::vector<int> lineWidth;
		std::vector<int> lineHeight;
		size_t getNumLines();
		size_t getTextWidth(int line = -1);
		size_t getTextHeight(int line = -1);
	};

	struct textContext
	{
		Font* textFont;
		Font* smallFont;
		Language* textLanguage;
		size_t width;
		size_t height;
		bool wrap;
		bool indent;
	};

	textInfo processText(const std::string& Text, const textContext& Context);
}
