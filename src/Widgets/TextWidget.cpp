/*
 *  TextWidget.cpp
 *  cefixWidgets
 *
 *  Created by Stephan Huber on 03.06.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "TextWidget.h"
#include <sstream>

#include "AreaSliderWidget.h"
#include "HorizontalSliderWidget.h"
#include "VerticalSliderWidget.h"
#include <cefix/WidgetFactory.h>

namespace cefix {

TextWidget::TextWidget(cefix::PropertyList* pl) : 
	AbstractWidget(pl->get("id")->asString()), 
	_pos(pl->get("position")->asVec3()),
	_precision(6) 
{
	
	std::string fontname = pl->get("font")->asString();
	if (fontname.empty()) fontname = "system.xml";
	float fontsize = pl->get("fontsize")->asFloat();
	_textGeode = new cefix::Utf8TextGeode(fontname, fontsize);
	_textGeode->setDataVariance(osg::Object::DYNAMIC);
	_textGeode->setPosition(_pos);
	
	if (pl->hasKey("color"))
		_textGeode->setTextColor(pl->get("color")->asVec4());
	else
		_textGeode->setTextColor(osg::Vec4(1,1,1,1));
	
	if (pl->hasKey("precision"))
		_precision = pl->get("precision")->asInt();
	
	if (pl->hasKey("alignment")) {
		std::string alignment(strToLower(pl->get("alignment")->asString()));
		if (alignment=="center")
			_textGeode->setTextAlignment(cefix::Utf8TextGeode::RenderIterator::ALIGN_CENTER);
		else if (alignment=="right")
			_textGeode->setTextAlignment(cefix::Utf8TextGeode::RenderIterator::ALIGN_RIGHT);
	}
	_textGeode->setText(pl->get("caption")->asString());
	_textGeode->refresh();
	
}

void TextWidget::respondToAction(const std::string& action, AbstractWidget* widget) {
	std::ostringstream ss;
	ss << std::setprecision(_precision);
	if (action == SliderWidget::Actions::valueChanged()) {
		{ 			
			HorizontalSliderWidget* s = dynamic_cast<HorizontalSliderWidget*>(widget);
			if (s) {
				double v = s->getValue();
				ss << v;
			}
		}
		{ 			
			VerticalSliderWidget* s = dynamic_cast<VerticalSliderWidget*>(widget);
			if (s) {
				double v = s->getValue();
				ss << v;
			}
		}
		
		{ 
			AreaSliderWidget* s = dynamic_cast<AreaSliderWidget*>(widget);
			if (s) {
				osg::Vec2 v = s->getValue();
				ss << v[0] << "/" << v[1];
			}
		}
		
		_textGeode->setText(ss.str());
	}
}


TextWidget::~TextWidget() {
	// osg::notify(osg::ALWAYS) << "destructing TextWidget: " << ((_textGeode.valid()) ? _textGeode->getText() : "./.") << std::endl;
}

}

