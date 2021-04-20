#include "LayoutGroup.h"


void OpenXcom::LayoutGroup::setX(int x)
{
	Surface::setX(x);
	_driver.ApplyLayout();
}

void OpenXcom::LayoutGroup::setY(int y)
{
	Surface::setY(y);
	_driver.ApplyLayout();
}

void OpenXcom::LayoutGroup::setWidth(int width)
{
	Surface::setWidth(width);
	_driver.ApplyLayout();
}

void OpenXcom::LayoutGroup::setHeight(int height)
{
	Surface::setHeight(height);
	_driver.ApplyLayout();
}
