/*
 *  TextButtonWidget.cpp
 *  cefixWidgets
 *
 *  Created by Stephan Huber on 04.06.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "TextButtonWidget.h"
#include <cefix/WidgetFactory.h>
namespace cefix {
	
	TextButtonWidget::TextButtonWidget(const std::string& identifier, const osg::Vec3& pos, const osg::Vec2& size, std::string caption, AbstractWidget::Responder* responder) :
		ButtonWidget(identifier, pos, size, responder),
		_caption(caption)
	{
	}
}

