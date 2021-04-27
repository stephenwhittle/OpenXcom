#pragma once
#include <vector>

namespace OpenXcom
{
	class Surface;



	enum class LayoutAlignment
	{
		Stretch,
		Center,
		Default
	};
	struct LayoutParam
	{

		Surface* target = nullptr;
		bool proportional = false;
		float desiredX = 100;
		float desiredY = 100;
		int originalX = 100;
		int originalY = 100;
		LayoutAlignment alignment = LayoutAlignment::Default;

		LayoutParam(Surface* Widget);
		/// @brief indicates the widget's size should not be altered at all, only its position
		LayoutParam& KeepSize();
		/// @brief indicates the widget should be proportionally resized in the remaining space
		/// along the primary axis after absolute size widgets have space allocated
		LayoutParam& Proportional(float XProportion, float YProportion);
		/// @brief indicates the widget should have its size along the primary axis fixed to
		/// the specified value
		LayoutParam& Absolute(float XSize, float YSize);
		/// @brief makes the off-axis size of the widget equal to the maximum available space
		LayoutParam& OtherAxisStretch();
		/// @brief positions the widget along the center of the off-axis
		LayoutParam& OtherAxisCenter();

		Surface* getWidget() const;
	};

	enum class LayoutDirection
	{
		Horizontal,
		Vertical
	};
	/// @brief Class to arrange a set of widgets either horizontally or vertically
	/// Given a parent surface, allocates space for all absolute sized widgets in the given direction, then proportionally
	/// allocates the remaining space between the proportional widgets based on the relative values given for their size in that axis.
	/// By default, places the widgets at the start of the other axis and does not alter their size. If specified will set the other
	/// axis' dimension to the maximum amount of free space, filling (OtherAxisStretch) or will centre the widget along that other axis
	/// (OtherAxisCenter).
	class LayoutDriver
	{
	private:
		Surface* _rootElement = nullptr;
		LayoutDirection _direction = LayoutDirection::Horizontal;
		std::vector<LayoutParam> _childElements;
		struct
		{
			int Top = 0;
			int Bottom = 0;
			int Left = 0;
			int Right = 0;
			int HPadding() { return Left + Right; }
			int VPadding() { return Top + Bottom; }
		} _padding;

		/// Helper method to make ApplyLayout a little more terse
		bool Horizontal();
	public:
		/// @brief Constructs a LayoutDriver that will use the specified primary axis and the
		///bounding box of the specified parent surface to lay children out based on
		/// LayoutParams rules
		template<typename... Params>
		LayoutDriver(LayoutDirection Direction, Surface* Parent, Params... Children)
			:_rootElement(Parent),
			_direction(Direction),
			_childElements{ Children... }
		{

		}
		/// @brief allocates the specified number of pixels around all edges of the parent widget
		/// before performing layout of children
		LayoutDriver& Padding(int AllValue);
		/// @brief allocates the specified number of pixels around all edges of the parent widget
		/// before performing layout of children
		LayoutDriver& Padding(int VPadding, int HPadding);
		/// @brief resizes and positions the widgets that were wrapped in the LayoutParams passed to the constructor
		void ApplyLayout();
	};

}
