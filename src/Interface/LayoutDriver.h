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
		LayoutParam& KeepSize();
		LayoutParam& Proportional(float XProportion, float YProportion);

		LayoutParam& Absolute(float XSize, float YSize);
		LayoutParam& OtherAxisStretch();
		LayoutParam& OtherAxisCenter();
		Surface* getWidget() const;
	};

	enum class LayoutDirection
	{
		Horizontal,
		Vertical
	};

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
		template<typename... Params>
		LayoutDriver(LayoutDirection Direction, Surface* Parent, Params... Children)
			:_rootElement(Parent),
			_direction(Direction),
			_childElements{ Children... }
		{

		}

		LayoutDriver& Padding(int AllValue);
		LayoutDriver& Padding(int VPadding, int HPadding);
		void ApplyLayout();
	};

}
