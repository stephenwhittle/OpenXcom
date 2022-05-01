#include "LayoutDriver.h"
#include "../Engine/Surface.h"


OpenXcom::LayoutParam::LayoutParam(Surface* Widget) : target(Widget), originalX(Widget->getWidth()), originalY(Widget->getHeight())
{
}
OpenXcom::LayoutParam& OpenXcom::LayoutParam::KeepSize()
{
	proportional = false;
	desiredX = target->getWidth();
	desiredY = target->getHeight();
	return *this;
}
OpenXcom::LayoutParam& OpenXcom::LayoutParam::Proportional(float XProportion, float YProportion)
{
	proportional = true;
	desiredX = XProportion;
	desiredY = YProportion;
	return *this;
}

OpenXcom::LayoutParam& OpenXcom::LayoutParam::Absolute(float XSize, float YSize)
{
	proportional = false;
	desiredX = XSize;
	desiredY = YSize;
	return *this;
}

OpenXcom::LayoutParam& OpenXcom::LayoutParam::OtherAxisStretch()
{
	alignment = LayoutAlignment::Stretch;
	return *this;
}

OpenXcom::LayoutParam& OpenXcom::LayoutParam::OtherAxisCenter()
{
	alignment = LayoutAlignment::Center;
	return *this;
}

OpenXcom::Surface* OpenXcom::LayoutParam::getWidget() const
{
	return target;
}

bool OpenXcom::LayoutDriver::Horizontal()
{
	return _direction == LayoutDirection::Horizontal;
}

OpenXcom::LayoutDriver& OpenXcom::LayoutDriver::Padding(int VPadding, int HPadding)
{
	_padding.Top = VPadding;
	_padding.Bottom = VPadding;
	_padding.Left = HPadding;
	_padding.Right = HPadding;
	return *this;
}

OpenXcom::LayoutDriver& OpenXcom::LayoutDriver::Padding(int AllValue)
{
	_padding.Top = AllValue;
	_padding.Left = AllValue;
	_padding.Bottom = AllValue;
	_padding.Right = AllValue;
	return *this;
}

void OpenXcom::LayoutDriver::ApplyLayout()
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
			int CurrentPosition = Horizontal() ? _rootElement->getX() + _padding.Left : _rootElement->getY() + _padding.Top;

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
			int CurrentPosition = Horizontal() ? _rootElement->getX() + _padding.Left : _rootElement->getY() + _padding.Top;
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
