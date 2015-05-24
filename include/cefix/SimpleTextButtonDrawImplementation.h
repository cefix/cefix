/*
 *
 *      (\(\
 *     (='.')           cefix --
 *    o(_")")           a multipurpose library on top of OpenSceneGraph
 *  
 *
 *                      Copyright 2001-2011, stephanmaximilianhuber.com 
 *                      Stephan Maximilian Huber. 
 *
 *                      All rights reserved.
 *
 */

#ifndef SIMPLE_TEXT_BUTTON_DRAW_IMPLEMENTATION
#define SIMPLE_TEXT_BUTTON_DRAW_IMPLEMENTATION
#include <cefix/TextGeode.h>

#include "SimpleButtondrawImplementation.h"

namespace cefix{

class TextButtonWidget;

class CEFIX_EXPORT SimpleTextButtonDrawImplementation : public osg::Group {

	public: 
		SimpleTextButtonDrawImplementation(const osg::Vec3& pos, const osg::Vec2&size);
		
		void update(TextButtonWidget* widget);
		
		void setBackgroundColorForState(AbstractWidget::States state, osg::Vec4 c) { _btn->setBackgroundColorForState(state,c); }
		void setTextColorForState(AbstractWidget::States state, osg::Vec4 c) { _textColorMap[state] = c; }
		
		void setCallback(cefix::Functor1ParamAbstractT<void, AbstractWidget::EventTypes>* fn) { _btn->setCallback(fn); }
		
		cefix::Utf8TextGeode* getTextGeode() { return _textGeode.get(); }
		
		void setTextAlignment (Utf8TextGeode::TextAlignment alignment, osg::Vec2 offset);

		void setOffset(const osg::Vec2& offset);
		
		void applyPropertyList(cefix::PropertyList* pl);

	
	protected:
		SimpleButtonDrawImplementation::ColorMap	_textColorMap;
		osg::ref_ptr<cefix::Utf8TextGeode>	_textGeode;
		osg::ref_ptr<SimpleButtonDrawImplementation> _btn;
		osg::Vec2 _textOffset;

};

} // end of Namespace


#endif