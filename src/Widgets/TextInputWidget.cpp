/*
 *  TextInputWidget.cpp
 *  cefixcvTemplate
 *
 *  Created by Stephan Huber on 29.10.08.
 *  Copyright 2008 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "TextInputWidget.h"
#include "TextInputBaseWidget.h"
#include <cefix/WidgetFactory.h>
#include <cefix/Pickable.h>
#include <osg/Depth>
#include <osg/PolygonMode>

namespace cefix 
{




TextInputWidget::TextInputWidget(const std::string& id, const osg::Vec3& pos, const osg::Vec2& size, const std::wstring& text)
:	TextInputBaseWidget(id,pos,size),
	_input(text),
	_cursorPos(_input.size()+1),
	_hasFocus(true),
	_registered(false)
{
	
}
	
	
osg::Node* TextInputWidget::getNode() 
{ 
	return _impl->getNode(); 
}


void TextInputWidget::setParent(AbstractWidget* parent)
{
	TextInputBaseWidget::setParent(parent);
	
	if (getParent()) { 
		registerInputWidget();
	} else {
		unregisterInputWidget();
	}
}
	


void TextInputWidget::update() {
	if (_impl.valid()) _impl->update();
}

void TextInputWidget::computeCursor() 
{
	setCursorPos(_cursorPos); 
}

	
bool TextInputWidget::handleKey(int key) {
	bool handled(true);
	
	if (!_hasFocus) return false;
	std::wstring old_input(_input);
	
	int size = static_cast<int>(_input.size());
	
	switch (key) {
	
		case osgGA::GUIEventAdapter::KEY_Left: 
			setCursorPos(_cursorPos - 1);
			break;
		case osgGA::GUIEventAdapter::KEY_Right:
			setCursorPos(_cursorPos + 1);
			break;
		
		case osgGA::GUIEventAdapter::KEY_BackSpace:
			if ((_cursorPos >= size) && (size >=1)) 
			{
				// letztes Zeichen löschen
				_input = _input.substr(0, size - 2);
				setCursorPos(getCursorPosition()-1);
			}
			else if ((_cursorPos >= 0) &&  (_cursorPos < size)) 
			{
				//Zeichen links vom Cursor löschen
				_input = _input.substr(0,_cursorPos)+_input.substr(_cursorPos+1, size);
				setCursorPos(getCursorPosition()-1);
			}
			break;
		case osgGA::GUIEventAdapter::KEY_Delete:
		case osgGA::GUIEventAdapter::KEY_KP_Delete:
			if ((_cursorPos > 0) &&  (_cursorPos < size-1))
				_input = _input.substr(0,_cursorPos+1)+_input.substr(_cursorPos+2, size);
			break;
			
		default:
			if (key >= 0xff00) {
				handled = false;
			} else {
				// std::cout <<"key " << key << std::endl;
				// TODO hande utf?
				if (_cursorPos < size)
					_input.insert(_cursorPos+1, 1, static_cast<char>(key)); 
				else 
					_input += static_cast<char>(key);
				
				_cursorPos++;
			}
	}
	
	if (_input != old_input)
		informAttachedResponder(Actions::valueChanged());
	if (handled)
		update();
	return handled;
}
	
		
	
	
#pragma mark ---


class PulsatingCursorCallback : public osg::NodeCallback {

public:
	PulsatingCursorCallback(SimpleTextInputWidgetImplementation* impl) : osg::NodeCallback(), _impl(impl) {}
	
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {
		_impl->pulsateCursor();
		traverse(node, nv);
	}
private:
	SimpleTextInputWidgetImplementation* _impl;
};


	
SimpleTextInputWidgetImplementation::SimpleTextInputWidgetImplementation(TextInputWidget* w) 
:	TextInputWidget::Implementation(w),
	_inset(2,2),
	_textColor(osg::Vec4(1,1,1,1)),
	_cursorColor(osg::Vec4(1,1,1,1)),
	_frameColor(osg::Vec4(1,1,1,1)),
	_fontName("system.xml"),
	_fontsize(0.0f)
{
	_group = new osg::Group();
	
	osg::Geode* geode = new osg::Geode();
	
	_frame = new cefix::Quad2DGeometry(0,0,0,0);
	_cursor = new cefix::Quad2DGeometry(0,0,0,0);
	
	geode->addDrawable(_frame.get());
	geode->addDrawable(_cursor.get());
	geode->setUpdateCallback(new PulsatingCursorCallback(this));
	geode->setUserData(new TextInputWidget::Pickable<SimpleTextInputWidgetImplementation>(this));
	
	_cursor->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
	_cursor->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	
	osg::Depth* d = new osg::Depth();
	d->setWriteMask(false);
	
	_cursor->getOrCreateStateSet()->setAttributeAndModes( d );
	_cursor->setColor(_cursorColor);
	
	_text = new cefix::UnicodeTextGeode("system.xml", 0);
	
	_frame->getOrCreateStateSet()->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE));
	
	_group->addChild(geode);
	_group->addChild(_text.get());
	
}

	

void SimpleTextInputWidgetImplementation::update()
{

	//std::cout << "cursorpos: " << getWidget()->getCursorPosition() << std::endl;
	_text->setTextColor(_textColor);
	_text->setText(getWidget()->getValue());
	osg::Vec3 p = getWidget()->getPosition();
	osg::Vec2 s = getWidget()->getSize();
	
	int cp = getWidget()->getCursorPosition();
	
	float dy = (cp >= 0) ? _text->getFont()->getWidth( getWidget()->getValue().substr(0, cp+1)) : 0;
	
	//std::cout << "dy: " << dy << " from: " << getWidget()->getValue().substr(0, cp+1) << std::endl;
	_cursor->setRect(p[0] + _inset[0] + dy, p[1] + 1, 1, s[1] - (2));
	_cursor->setLocZ(p[2]+1);
	
	_frame->setRect(p[0],p[1], s[0],s[1]);
	_frame->setLocZ(p[2]);
	_frame->setColor(_frameColor);
	_text->setPosition(osg::Vec3(p + osg::Vec3(_inset[0], _inset[1], 0)));
	
	
}

void SimpleTextInputWidgetImplementation::applyPropertyList (cefix::PropertyList *pl)
{
	if (pl->hasKey("fontname")) _fontName = pl->get("fontname")->asString();
	if (pl->hasKey("fontsize")) _fontsize = pl->get("fontsize")->asFloat();
	if (pl->hasKey("textcolor")) _textColor = pl->get("textcolor")->asVec4();
	if (pl->hasKey("inset")) _inset = pl->get("inset")->asVec2();
	
	_text->setFont(_fontName);
	_text->setFontSize(_fontsize);
}



// static WidgetFactory::RegisterProxy< SimpleTextInputWidget > simple_imput_widget_proxy("simpletextinputwidget", "id position value");

}
