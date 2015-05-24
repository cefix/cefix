/*
 *  SimpleCheckboxDrawImplementation.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 20.11.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */
#include <cefix/PropertyList.h>
#include "SimpleCheckboxDrawImplementation.h"
#include <cefix/CheckboxWidget.h>
namespace cefix {

SimpleCheckboxDrawImplementation::SimpleCheckboxDrawImplementation(const osg::Vec3& pos, const osg::Vec2& size) :SimpleTextButtonDrawImplementation(pos, size) 
{
	setTextAlignment(cefix::StandardTextRenderIterator::ALIGN_LEFT, osg::Vec2(size[0] + 10, 0) );
	setBackgroundColorForState(cefix::AbstractWidget::SELECTED, osg::Vec4(1,1,1,0.8));
	setBackgroundColorForState(cefix::AbstractWidget::SELECTED_OVER, osg::Vec4(1,1,1,1.0));
}


void SimpleCheckboxDrawImplementation::update(CheckboxWidget* widget)
{
	SimpleTextButtonDrawImplementation::update(widget);
}
		
void SimpleCheckboxDrawImplementation::applyPropertyList(cefix::PropertyList* pl) 
{
	osg::Vec2 size = pl->get("size")->asVec2();
	setTextAlignment(cefix::StandardTextRenderIterator::ALIGN_LEFT, osg::Vec2(size[0] + 10, 0) );

	SimpleTextButtonDrawImplementation::applyPropertyList(pl);
}


} 