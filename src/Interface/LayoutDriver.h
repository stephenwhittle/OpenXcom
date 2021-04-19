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
		/// Helper method to make ApplyLayout a little more terse
		bool Horizontal() { return _direction == LayoutDirection::Horizontal; }
	public:
		template<typename... Params>
		LayoutDriver(LayoutDirection Direction, Surface* Parent, Params... Children)
			: _rootElement(Parent),
			_direction(Direction),
			_childElements{ Children... } {};


		void ApplyLayout()
		{
			{
				int AvailableSpace = Horizontal() ? _rootElement->getWidth() : _rootElement->getHeight();
				int OffAxisAvailableSpace = Horizontal() ? _rootElement->getHeight() : _rootElement->getWidth();
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
					int CurrentPosition = 0;

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
									Child.getWidget()->setY(_rootElement->getY());
								}
								else if (Child.alignment == LayoutAlignment::Center)
								{
									int OffAxisCenter = _rootElement->getY() + _rootElement->getHeight() / 2;
									Child.getWidget()->setY(OffAxisCenter - Child.getWidget()->getHeight() / 2);
								}
							}
							else
							{
								Child.getWidget()->setHeight(DesiredDimension);
								Child.getWidget()->setY(CurrentPosition);
								if (Child.alignment == LayoutAlignment::Stretch)
								{
									Child.getWidget()->setWidth(OffAxisAvailableSpace);
									Child.getWidget()->setX(_rootElement->getX());
								}
								else if (Child.alignment == LayoutAlignment::Center)
								{
									int OffAxisCenter = _rootElement->getX() + _rootElement->getWidth() / 2;
									Child.getWidget()->setX(OffAxisCenter - Child.getWidget()->getWidth() / 2);
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
									Child.getWidget()->setY(_rootElement->getY());
								}
								else if (Child.alignment == LayoutAlignment::Center)
								{
									int OffAxisCenter = _rootElement->getY() + _rootElement->getHeight() / 2;
									Child.getWidget()->setY(OffAxisCenter - Child.getWidget()->getHeight() / 2);
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
									Child.getWidget()->setX(_rootElement->getX());
								}
								else if (Child.alignment == LayoutAlignment::Center)
								{
									int OffAxisCenter = _rootElement->getX() + _rootElement->getWidth() / 2;
									Child.getWidget()->setX(OffAxisCenter - Child.getWidget()->getWidth() / 2);
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
									Child.getWidget()->setY(_rootElement->getY());
								}
								else if (Child.alignment == LayoutAlignment::Center)
								{
									int OffAxisCenter = _rootElement->getY() + _rootElement->getHeight() / 2;
									Child.getWidget()->setY(OffAxisCenter - Child.getWidget()->getHeight() / 2);
								}
							}
							else
							{
								Child.getWidget()->setHeight(DesiredDimension);
								Child.getWidget()->setY(CurrentPosition);
								if (Child.alignment == LayoutAlignment::Stretch)
								{
									Child.getWidget()->setWidth(OffAxisAvailableSpace);
									Child.getWidget()->setX(_rootElement->getX());
								}
								else if (Child.alignment == LayoutAlignment::Center)
								{
									int OffAxisCenter = _rootElement->getX() + _rootElement->getWidth() / 2;
									Child.getWidget()->setX(OffAxisCenter - Child.getWidget()->getWidth() / 2);
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
									Child.getWidget()->setY(_rootElement->getY());
								}
								else if (Child.alignment == LayoutAlignment::Center)
								{
									int OffAxisCenter = _rootElement->getY() + _rootElement->getHeight() / 2;
									Child.getWidget()->setY(OffAxisCenter - Child.getWidget()->getHeight() / 2);
								}
							}
							else
							{
								Child.getWidget()->setHeight(DesiredDimension);
								Child.getWidget()->setY(CurrentPosition);
								if (Child.alignment == LayoutAlignment::Stretch)
								{
									Child.getWidget()->setWidth(OffAxisAvailableSpace);
									Child.getWidget()->setX(_rootElement->getX());
								}
								else if (Child.alignment == LayoutAlignment::Center)
								{
									int OffAxisCenter = _rootElement->getX() + _rootElement->getWidth() / 2;
									Child.getWidget()->setX(OffAxisCenter - Child.getWidget()->getWidth() / 2);
								}
							}

							CurrentPosition += DesiredDimension;
						}
					}
					/// get how much we need to scale down by
					/// absolute ones get their desired size scaled by that factor, and then set
					/// proportional ones 
				}
			}
		}
	};


}
