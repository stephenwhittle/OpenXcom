#pragma once
#include <deque>
/// <summary>
/// Fixed-size buffer of the most recent x messages
/// Intended for use in our graphical tests
/// </summary>
class LogBuffer
{
	std::deque<std::string> elements;

public:
	void Insert(std::string Item)
	{
		elements.push_back(Item);
		while (elements.size() > 10)
		{
			elements.pop_front();
		}
	}
	void Clear()
	{
		elements.clear();
	}

	auto begin() const
	{
		return elements.begin();
	}
	
	auto end() const
	{
		return elements.end();
	}

};