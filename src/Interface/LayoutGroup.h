#pragma once
#include "../Engine/Surface.h"
#include "LayoutDriver.h"

namespace OpenXcom
{

	class LayoutGroup : public Surface
	{
	private:
		LayoutDriver _driver;

		template<typename... Params>
		LayoutGroup(int width, int height, LayoutDirection Direction, Params... Children)
		: Surface(width, height),
		_driver(Direction, this, Children...) {};


	public:
		template<typename... Params>
		static LayoutGroup* Vertical(int width, int height, Params... Children)
		{
			return new LayoutGroup(width, height, OpenXcom::LayoutDirection::Vertical, std::forward<Params>(Children)...);
		}

		template<typename... Params>
		static LayoutGroup* Horizontal(int width, int height, Params... Children)
		{
			return new LayoutGroup(width, height, OpenXcom::LayoutDirection::Horizontal, std::forward<Params>(Children)...);
		}

		/// Surface methods
		void setX(int x) override;
		void setY(int y) override;
		void setWidth(int width) override;
		void setHeight(int height) override;
	};

}
