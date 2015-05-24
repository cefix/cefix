/*
 *  TextInputBaseWidget.cpp
 *  cefixcvTemplate
 *
 *  Created by Stephan Huber on 31.10.08.
 *  Copyright 2008 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "TextInputBaseWidget.h"
#include "TextInputWidgetManager.h"

namespace cefix {

TextInputBaseWidget::TextInputBaseWidget(const std::string& id, const osg::Vec3& pos, const osg::Vec2& size)
:	RectControlWidget(id, pos, size),
	_hasFocus(false),
	_registered(false)
{

}

TextInputBaseWidget::~TextInputBaseWidget() 
{
	unregisterInputWidget();
}

void TextInputBaseWidget::setFocus(bool b, bool calledFromManager) 
{
	if (b && !calledFromManager) TextInputWidgetManager::instance()->setFocus(this);
	_hasFocus = b;
	update();
}
void TextInputBaseWidget::registerInputWidget() {
	if (!_registered) {
		_registered = true;
		TextInputWidgetManager::instance()->registerTextInputWidget(this);
	}
}

void TextInputBaseWidget::unregisterInputWidget() 
{
	if (_registered) {
		_registered = false;
		TextInputWidgetManager::instance()->unregisterTextInputWidget(this);
	}
}

}