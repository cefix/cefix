/*
 *  SimpleTextButtonDrawInmplementation.cpp
 *  cefixWidgets
 *
 *  Created by Stephan Huber on 04.06.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "SimpleTextButtonDrawImplementation.h"
#include <cefix/DataFactory.h>
#include <cefix/TextButtonWidget.h>
#include <cefix/SimpleWidgetDrawImplementationDefaults.h>

namespace cefix {

SimpleTextButtonDrawImplementation::SimpleTextButtonDrawImplementation(const osg::Vec3& pos, const osg::Vec2&size) :
	osg::Group()	
{
	
	_btn = new SimpleButtonDrawImplementation(pos, size);
    
    SimpleWidgetDrawImplementationDefaults* defaults = SimpleWidgetDrawImplementationDefaults::instance();

	AbstractWidget::States states[6] = {
        AbstractWidget::DISABLED, 
        AbstractWidget::NORMAL,
        AbstractWidget::NORMAL_OVER,
        AbstractWidget::SELECTED,
        AbstractWidget::SELECTED_OVER,
        AbstractWidget::PRESSED
    };
    
    for(unsigned int i = 0; i < 6; ++i) {
        setTextColorForState(states[i], defaults->getBtnTextColorFor(states[i]));
    }

	_textGeode = new cefix::Utf8TextGeode(defaults->getBtnFontName(), defaults->getBtnFontSize());
	_textGeode->setDataVariance(osg::Object::DYNAMIC);
	_textGeode->setSubPixelAlignment(false);
	_textGeode->setTextAlignment(defaults->getBtnTextAlignment() );
	
	addChild(_btn.get());
	addChild(_textGeode.get());
	
	setOffset(defaults->getBtnTextOffset());
	
}


void SimpleTextButtonDrawImplementation::setTextAlignment (Utf8TextGeode::TextAlignment alignment, osg::Vec2 offset){
	
	_textGeode->setTextAlignment(alignment);
	setOffset(offset);

}

void SimpleTextButtonDrawImplementation::setOffset(const osg::Vec2& offset) {
	_textOffset = offset;
}


void SimpleTextButtonDrawImplementation::update(TextButtonWidget* widget) {
	
	_btn->update(widget);
	
	_textGeode->setText(widget->getCaption());
	_textGeode->refresh();
	
	Utf8TextGeode::TextAlignment tAlignment = _textGeode->getTextAlignment();
	
	// das ist etwas genauer als _textGeode->getHeight(), getHeight liefert die Boundingbox zurŸck, getMajuscleHeight die Hšhe von Grossbuchstaben
	
	float h = _textGeode->getFont()->getMajuscleHeight() + (_textGeode->getRenderedLineCount() - 1) * _textGeode->getLineSpacing();
	
	osg::Vec2 size = (_btn->getSize());
	osg::Vec3 tTextPosition;
	// std::cout <<"size" << size[1] << " height: " << h << std::endl ;
	
	switch(tAlignment){
		case Utf8TextGeode::RenderIterator::ALIGN_LEFT:
			tTextPosition = _btn->getPosition() + osg::Vec3(_textOffset[0] + 0, _textOffset[1] + ((size[1] - h ) / 2.0), 1);
			break;
		case Utf8TextGeode::RenderIterator::ALIGN_CENTER:
			tTextPosition = _btn->getPosition() + osg::Vec3(_textOffset[0] + (size[0] / 2.0), _textOffset[1] + ((size[1] - h ) / 2.0), 1);
			break;
		case Utf8TextGeode::RenderIterator::ALIGN_RIGHT:
			tTextPosition = _btn->getPosition() + osg::Vec3(_textOffset[0] + size[0], _textOffset[1] + ((size[1] - h ) / 2.0), 1);
			break;
	}
	
	_textGeode->setPosition(tTextPosition);
	
	if (_textColorMap.find(widget->getState()) != _textColorMap.end())
		_textGeode->setTextColor(_textColorMap[widget->getState()]);

}

void SimpleTextButtonDrawImplementation::applyPropertyList(cefix::PropertyList* pl) 
{ 
	
	if (pl && pl->hasKey("prefs"))
		applyPropertyList(cefix::DataFactory::instance()->getPreferencesPropertyListFor(pl->get("prefs")->asString()));
		
	if (pl == NULL)	return;
	
	static std::map<std::string, cefix::AbstractWidget::States> statemap;
	if (statemap.size() == 0) {
		statemap["disabled"] = AbstractWidget::DISABLED;
		statemap["normal"] = AbstractWidget::NORMAL;
		statemap["normal_over"] = AbstractWidget::NORMAL_OVER;
		statemap["selected"] = AbstractWidget::SELECTED;
		statemap["selected_over"] = AbstractWidget::SELECTED_OVER;
		statemap["pressed"] = AbstractWidget::PRESSED;
	}
	if (pl->hasKey("textColors") && (pl->get("textColors")->getType() == cefix::AbstractProperty::PROPERTY_LIST)) {
		cefix::PropertyList* subpl = pl->get("textColors")->asPropertyList();
		for(std::map<std::string, cefix::AbstractWidget::States>::iterator i = statemap.begin(); i != statemap.end(); ++i) {
			if (subpl->hasKey(i->first))
				setTextColorForState(i->second, subpl->get(i->first)->asVec4());
		}
	}
	
	std::string fontname = pl->get("font")->asString();
	if (fontname.empty()) fontname = "system.xml";
	float fontsize = pl->get("fontsize")->asFloat();
	
	_textGeode->setFont(fontname);
	_textGeode->setFontSize(fontsize);
	
	if (pl->hasKey("textOffset"))
		_textOffset = pl->get("textOffset")->asVec2();
	
	if (pl->hasKey("alignment")) {
		std::string alignment(strToLower(pl->get("alignment")->asString()));
		if (alignment=="center")
			setTextAlignment(cefix::Utf8TextGeode::RenderIterator::ALIGN_CENTER, _textOffset);
		else if (alignment=="right")
			setTextAlignment(cefix::Utf8TextGeode::RenderIterator::ALIGN_RIGHT, _textOffset);
		else
			setTextAlignment(cefix::Utf8TextGeode::RenderIterator::ALIGN_LEFT, _textOffset);
	}
	
	_btn->applyPropertyList(pl);

}

}