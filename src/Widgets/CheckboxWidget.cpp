/*
 *  CheckboxWidget.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 20.11.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "CheckboxWidget.h"
#include "WidgetFactory.h"

namespace cefix {
CheckboxWidget::CheckboxWidget(const std::string& identifier, const osg::Vec3& pos, const osg::Vec2& size, const std::string& caption, Responder* responder)  
	: 
	TextButtonWidget(identifier, pos, size, caption, responder) 
{
	setSelectableFlag(true);
}

void CheckboxWidget::setSelected(bool f) 
{ 
	RadioButtonGroupWidget* radiobuttongroup = dynamic_cast<RadioButtonGroupWidget*>(getParent());
	if (radiobuttongroup) {
		if (f) {
			TextButtonWidget::setSelected(f);
			radiobuttongroup->setNewSelection(this);
		}
	} else {
		TextButtonWidget::setSelected(f);
	}
}
}

