#pragma once

namespace OpenXcom
{
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

		LayoutParam(Surface* Widget) : target(Widget), originalX(Widget->getWidth()), originalY(Widget->getHeight()) {};
		LayoutParam& Proportional(float XProportion, float YProportion)
		{
			proportional = true;
			desiredX = XProportion;
			desiredY = YProportion;
			return *this;
		}

		LayoutParam& Absolute(float XSize, float YSize)
		{
			proportional = false;
			desiredX = XSize;
			desiredY = YSize;
			return *this;
		}
		LayoutParam& OtherAxisStretch()
		{
			alignment = LayoutAlignment::Stretch;
			return *this;
		}
		LayoutParam& OtherAxisCenter()
		{
			alignment = LayoutAlignment::Center;
			return *this;
		}
		Surface* getWidget() const
		{
			return target;
		}
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
		bool Horizontal() { return _direction == LayoutDirection::Horizontal; }
	public:
		template<typename... Params>
		LayoutDriver(LayoutDirection Direction, Surface* Parent, Params... Children)
			: _rootElement(Parent),
			_direction(Direction),
			_childElements{ Children... } {};

		LayoutDriver& Padding(int AllValue)
		{
			_padding.Top = AllValue;
			_padding.Left = AllValue;
			_padding.Bottom = AllValue;
			_padding.Right = AllValue;
			return *this;
		}
		LayoutDriver& Padding(int VPadding, int HPadding)
		{
			_padding.Top = VPadding;
			_padding.Bottom = VPadding;
			_padding.Left = HPadding;
			_padding.Right = HPadding;
		}
		void ApplyLayout()
		{
			{
				int AvailableSpace = Horizontal() ? _rootElement->getWidth() - _padding.HPadding() : _rootElement->getHeight() - _padding.VPadding();
				int OffAxisAvailableSpace = Horizontal() ? _rootElement->getHeight() - _padding.VPadding() : _rootElement->getWidth() - _padding.HPadding();

				int OffAxisCenter = Horizontal() ?
					_rootElement->getY() + _padding.Top + (OffAxisAvailableSpace / 2) :
					_rootElement->getX() + _padding.Left + (OffAxisAvailableSpace / 2);

				//get the desired widths of all absolute elements

				int DesiredSpace = 0;
				float TotalFlexValues = 0;
				int NumberOfAbsoluteElements = 0;
				for (LayoutParam& Child : _childElements)
				{
					if (Child.proportional)
					{
						TotalFlexValues += Horizontal() ? Child.desiredX : Child.desiredY;
						//DesiredSpace += Horizontal() ? Child.originalX : Child.originalY;
					}
					else
					{
						NumberOfAbsoluteElements++;
						DesiredSpace += Horizontal() ? Child.desiredX : Child.desiredY;
					}

				}

				int RemainingSpace = AvailableSpace - DesiredSpace;
				if (RemainingSpace > 0)
				{
					int CurrentPosition = Horizontal() ? _padding.Left : _padding.Top;

					for (LayoutParam& Child : _childElements)
					{
						if (Child.proportional)
						{
							float ScalingFactor = (Horizontal() ? Child.desiredX : Child.desiredY) / TotalFlexValues;
							float DesiredDimension = RemainingSpace * ScalingFactor;
							if (Horizontal())
							{
								Child.getWidget()->setWidth(DesiredDimension);
								Child.getWidget()->setX(CurrentPosition);
								if (Child.alignment == LayoutAlignment::Stretch)
								{
									Child.getWidget()->setHeight(OffAxisAvailableSpace);
									Child.getWidget()->setY(_rootElement->getY() + _padding.Top);
								}
								else if (Child.alignment == LayoutAlignment::Center)
								{
									Child.getWidget()->setY(OffAxisCenter - Child.getWidget()->getHeight() / 2);
								}
								else
								{
									Child.getWidget()->setY(_rootElement->getY() + _padding.Top);
								}
							}
							else
							{
								Child.getWidget()->setHeight(DesiredDimension);
								Child.getWidget()->setY(CurrentPosition);
								if (Child.alignment == LayoutAlignment::Stretch)
								{
									Child.getWidget()->setWidth(OffAxisAvailableSpace);
									Child.getWidget()->setX(_rootElement->getX() + _padding.Left);
								}
								else if (Child.alignment == LayoutAlignment::Center)
								{
									Child.getWidget()->setX(OffAxisCenter - Child.getWidget()->getWidth() / 2);
								}
								else
								{
									Child.getWidget()->setX(_rootElement->getX() + _padding.Left);
								}

							}

							CurrentPosition += DesiredDimension;
						}
						else
						{
							if (Horizontal())
							{
								Child.getWidget()->setWidth(Child.desiredX);
								Child.getWidget()->setX(CurrentPosition);
								if (Child.alignment == LayoutAlignment::Stretch)
								{
									Child.getWidget()->setHeight(OffAxisAvailableSpace);
									Child.getWidget()->setY(_rootElement->getY() + _padding.Top);
								}
								else if (Child.alignment == LayoutAlignment::Center)
								{
									Child.getWidget()->setY(OffAxisCenter - Child.getWidget()->getHeight() / 2);
								}
								else
								{
									Child.getWidget()->setY(_rootElement->getY() + _padding.Top);
								}
								CurrentPosition += Child.desiredX;
							}
							else
							{
								Child.getWidget()->setHeight(Child.desiredY);
								Child.getWidget()->setY(CurrentPosition);
								if (Child.alignment == LayoutAlignment::Stretch)
								{
									Child.getWidget()->setWidth(OffAxisAvailableSpace);
									Child.getWidget()->setX(_rootElement->getX() + _padding.Left);
								}
								else if (Child.alignment == LayoutAlignment::Center)
								{
									Child.getWidget()->setX(OffAxisCenter - Child.getWidget()->getWidth() / 2);
								}
								else
								{
									Child.getWidget()->setX(_rootElement->getX() + _padding.Left);
								}

								CurrentPosition += Child.desiredY;
							}
						}
					}
				}
				else
				{
					int CurrentPosition = 0;
					float ShrinkFactor = AvailableSpace / (float)DesiredSpace;
					for (LayoutParam& Child : _childElements)
					{
						if (Child.proportional)
						{
							float ScalingFactor = (Horizontal() ? Child.desiredX : Child.desiredY) / (NumberOfAbsoluteElements + TotalFlexValues);
							float DesiredDimension = AvailableSpace * ScalingFactor;

							if (Horizontal())
							{
								Child.getWidget()->setWidth(DesiredDimension);
								Child.getWidget()->setX(CurrentPosition);
								if (Child.alignment == LayoutAlignment::Stretch)
								{
									Child.getWidget()->setHeight(OffAxisAvailableSpace);
									Child.getWidget()->setY(_rootElement->getY() + _padding.Top);
								}
								else if (Child.alignment == LayoutAlignment::Center)
								{
									Child.getWidget()->setY(OffAxisCenter - Child.getWidget()->getHeight() / 2);
								}
								else
								{
									Child.getWidget()->setY(_rootElement->getY() + _padding.Top);
								}
							}
							else
							{
								Child.getWidget()->setHeight(DesiredDimension);
								Child.getWidget()->setY(CurrentPosition);
								if (Child.alignment == LayoutAlignment::Stretch)
								{
									Child.getWidget()->setWidth(OffAxisAvailableSpace);
									Child.getWidget()->setX(_rootElement->getX() + _padding.Left);
								}
								else if (Child.alignment == LayoutAlignment::Center)
								{
									Child.getWidget()->setX(OffAxisCenter - Child.getWidget()->getWidth() / 2);
								}
								else
								{
									Child.getWidget()->setX(_rootElement->getX() + _padding.Left);
								}

							}

							CurrentPosition += DesiredDimension;
						}
						else
						{
							AvailableSpace / (NumberOfAbsoluteElements + TotalFlexValues);

							float ScalingFactor = 1.0f / (NumberOfAbsoluteElements + TotalFlexValues);
							float DesiredDimension = AvailableSpace * ScalingFactor;
							if (Horizontal())
							{
								Child.getWidget()->setWidth(DesiredDimension);
								Child.getWidget()->setX(CurrentPosition);
								if (Child.alignment == LayoutAlignment::Stretch)
								{
									Child.getWidget()->setHeight(OffAxisAvailableSpace);
									Child.getWidget()->setY(_rootElement->getY() + _padding.Top);
								}
								else if (Child.alignment == LayoutAlignment::Center)
								{
									Child.getWidget()->setY(OffAxisCenter - Child.getWidget()->getHeight() / 2);
								}
								else
								{
									Child.getWidget()->setY(_rootElement->getY() + _padding.Top);
								}
							}
							else
							{
								Child.getWidget()->setHeight(DesiredDimension);
								Child.getWidget()->setY(CurrentPosition);
								if (Child.alignment == LayoutAlignment::Stretch)
								{
									Child.getWidget()->setWidth(OffAxisAvailableSpace);
									Child.getWidget()->setX(_rootElement->getX() + _padding.Left);
								}
								else if (Child.alignment == LayoutAlignment::Center)
								{
									Child.getWidget()->setX(OffAxisCenter - Child.getWidget()->getWidth() / 2);
								}
								else
								{
									Child.getWidget()->setX(_rootElement->getX() + _padding.Left);
								}
							}

							CurrentPosition += DesiredDimension;
						}
					}
				}
			}
		}
	};


}
