/*
 *  SliderWidget.cpp
 *  cefixWidgets
 *
 *  Created by Stephan Huber on 03.06.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include <cefix/ConvertScreenCoord.h>
#include <cefix/Draggable.h>
#include <limits>

#include "SliderWidget.h"

namespace cefix {


void SliderDraggable::enter()
{
	if (_slider.valid()) _slider->handleMouseEvent(AbstractWidget::MOUSE_ENTER);
    Draggable::enter();
}

void SliderDraggable::leave()
{
	if (_slider.valid()) _slider->handleMouseEvent(AbstractWidget::MOUSE_LEAVE);
	Draggable::leave();
}

void SliderDraggable::down()
{
	_startEvent = getCurrentPointerData().event;
	if (_slider.valid()) _slider->handleMouseEvent(AbstractWidget::MOUSE_DOWN);
	Draggable::down();
}

void SliderDraggable::up(bool inside)
{
	if (_slider.valid()) _slider->handleMouseEvent(inside ? AbstractWidget::MOUSE_UP : AbstractWidget::MOUSE_UP_OUTSIDE);
	Draggable::up(inside);
}

bool SliderDraggable::dragStarted() { 
	stopPropagation();
	_p = getCurrentHit().localIntersectionPoint - _origin;
	return true; 
}

bool SliderDraggable::drag(const osg::Vec3& delta) 
{ 
	
	if (_slider.valid()) {
		_divisor = 1.0f;
	
		if (_startEvent) 
		{
			if (_startEvent->getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_SHIFT)
				_divisor = 10;
			else if (_startEvent->getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_ALT)
				_divisor = 100;
		}
		_slider->dragCallback(_p+(delta / _divisor)); 
		//std::cout << _startEvent->getModKeyMask() <<" " <<_divisor << " " << delta << " " << std::endl;
	}
	return true; 
}

void SliderWidget::findNearestValue()
{
	if (_allowedValues.size() == 0)
		return;
	//std::cout << "old: " << _value << std::endl;
	double min(std::numeric_limits<double>::max());
	osg::Vec2 result;
	for(std::vector<osg::Vec2>::iterator i = _allowedValues.begin(); i != _allowedValues.end(); ++i) 
	{
		osg::Vec2 d((*i) - _value);
		if (d.length2() < min) {
			min = d.length2();
			result = *i;
		}
	}
	_value = result;
	//std::cout << "new: " << _value << std::endl;
}



}