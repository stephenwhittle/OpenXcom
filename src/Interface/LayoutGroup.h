#pragma once
#include "../Engine/Surface.h"
#include "LayoutDriver.h"

namespace OpenXcom
{
	/// @brief Surface that will manage a set of sibling widgets with a layout driver
	/// This allows for the nesting of layout drivers
	/// Setting the position or size of this surface will cause the managed sibling widgets
	/// to have their position and size recalculated
	class LayoutGroup : public Surface
	{
	private:
		LayoutDriver _driver;

		template<typename... Params>
		LayoutGroup(int width, int height, LayoutDirection Direction, Params... Children)
		: Surface(width, height),
		_driver(Direction, this, Children...) {};


	public:
		/// @brief Static factory method for constructing a LayoutGroup with a layout driver controlling the vertical axis as primary
		template<typename... Params>
		static LayoutGroup* Vertical(int width, int height, Params... Children)
		{
			return new LayoutGroup(width, height, OpenXcom::LayoutDirection::Vertical, std::forward<Params>(Children)...);
		}

		/// @brief Static factory method for constructing a LayoutGroup with a layout driver controlling the horizontal axis as primary
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
