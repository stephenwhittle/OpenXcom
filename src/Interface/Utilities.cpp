#include "Utilities.h"

#include "../Engine/Font.h"
#include "../Engine/Language.h"
#include "../Engine/Unicode.h"
#include <vector>


OpenXcom::textInfo OpenXcom::processText(const std::string& Text, const textContext& Context)
{
	if (Context.textFont == 0 || Context.textLanguage == 0)
	{
		return {};
	}
	textInfo processedInfo{};

	processedInfo.processedText = Unicode::convUtf8ToUtf32(Text);

	int width = 0, word = 0;
	size_t space = 0, textIndentation = 0;
	bool start = true;
	Font* font = Context.textFont;
	UString& str = processedInfo.processedText;

	// Go through the text character by character
	for (size_t c = 0; c <= str.size(); ++c)
	{
		// End of the line
		if (c == str.size() || Unicode::isLinebreak(str[c]))
		{
			// Add line measurements for alignment later
			processedInfo.lineWidth.push_back(width);
			processedInfo.lineHeight.push_back(font->getCharSize('\n').h);
			width = 0;
			word = 0;
			start = true;

			if (c == str.size())
			{
				break;
			}
			else if (str[c] == Unicode::TOK_NL_SMALL && Context.smallFont)
			{
				font = Context.smallFont;
			}
		}
		// Keep track of spaces for wordwrapping
		else if (Unicode::isSpace(str[c]) || Unicode::isSeparator(str[c]))
		{
			// Store existing indentation
			if (c == textIndentation)
			{
				textIndentation++;
			}
			space = c;
			width += font->getCharSize(str[c]).w;
			word = 0;
			start = false;
		}
		// Keep track of the width of the last line and word
		else if (str[c] != Unicode::TOK_COLOR_FLIP)
		{
			int charWidth = font->getCharSize(str[c]).w;

			width += charWidth;
			word += charWidth;

			// Wordwrap if the last word doesn't fit the line
			if (Context.wrap && width >= Context.width && (!start || Context.textLanguage->getTextWrapping() == WRAP_LETTERS))
			{
				size_t indentLocation = c;
				if (Context.textLanguage->getTextWrapping() == WRAP_WORDS || Unicode::isSpace(str[c]))
				{
					// Go back to the last space and put a linebreak there
					width -= word;
					indentLocation = space;
					if (Unicode::isSpace(str[space]))
					{
						width -= font->getCharSize(str[space]).w;
						str[space] = '\n';
					}
					else
					{
						str.insert(space + 1, 1, '\n');
						indentLocation++;
					}
				}
				else if (Context.textLanguage->getTextWrapping() == WRAP_LETTERS)
				{
					// Go back to the last letter and put a linebreak there
					str.insert(c, 1, '\n');
					width -= charWidth;
				}

				// Keep initial indentation of text
				if (textIndentation > 0)
				{
					str.insert(indentLocation + 1, textIndentation, '\t');
					indentLocation += textIndentation;
				}
				// Indent due to word wrap.
				if (Context.indent)
				{
					str.insert(indentLocation + 1, 1, '\t');
					width += font->getCharSize('\t').w;
				}

				processedInfo.lineWidth.push_back(width);
				processedInfo.lineHeight.push_back(font->getCharSize('\n').h);
				if (Context.textLanguage->getTextWrapping() == WRAP_WORDS)
				{
					width = word;
				}
				else if (Context.textLanguage->getTextWrapping() == WRAP_LETTERS)
				{
					width = 0;
				}
				start = true;
			}
		}
	}
	return processedInfo;
}

size_t OpenXcom::textInfo::getNumLines()
{
	return lineHeight.size();
}

size_t OpenXcom::textInfo::getTextWidth(int line /*= -1*/)
{
	if (line == -1)
	{
		int width = 0;
		for (std::vector<int>::const_iterator i = lineWidth.begin(); i != lineWidth.end(); ++i)
		{
			if (*i > width)
			{
				width = *i;
			}
		}
		return width;
	}
	else
	{
		return lineWidth[line];
	}
}

size_t OpenXcom::textInfo::getTextHeight(int line /*= -1*/)
{
	if (line == -1)
	{
		int height = 0;
		for (std::vector<int>::const_iterator i = lineHeight.begin(); i != lineHeight.end(); ++i)
		{
			height += *i;
		}
		return height;
	}
	else
	{
		return lineHeight[line];
	}
}
