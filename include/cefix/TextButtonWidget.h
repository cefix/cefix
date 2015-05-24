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


#ifndef TEXT_BUTTON_WIDGET_HEADER
#define TEXT_BUTTON_WIDGET_HEADER

#include <cefix/ButtonWidget.h>
#include <cefix/SimpleTextButtonDrawImplementation.h>
#include <cefix/PropertyList.h>
#include <cefix/Functor.h>

namespace cefix {

/** a TextButton-implementation */
class CEFIX_EXPORT TextButtonWidget : public ButtonWidget {
	protected:
		/** ctor */
		TextButtonWidget(const std::string& identifier, const osg::Vec3& pos, const osg::Vec2& size, std::string caption, AbstractWidget::Responder* responder = NULL);		
	public:
		/** get the button's caption */
		inline const std::string& getCaption() { return _caption; }
		
		/** set the button's caption */
		inline void setCaption(const std::string& s) { _caption = s; update(); }
	
	private:
		std::string _caption;

};


/** IMplementation-template, bringing together a TextButtonWidget-class and a Implementation class */
template< class TextButtonWidgetClass, class DrawImplementationClass = SimpleTextButtonDrawImplementation> class TextButtonWidgetT : public TextButtonWidgetClass {

	public:
		/** ctor */
		TextButtonWidgetT(const std::string& identifier, const osg::Vec3& pos, const osg::Vec2& size, const std::string& caption, AbstractWidget::Responder* responder = NULL) : 
			TextButtonWidgetClass(identifier, pos, size, caption, responder)

		{
			_drawImplementation = new DrawImplementationClass(pos, size);
			//cefix::Functor1ParamT<void, AbstractWidget::EventTypes, TextButtonWidgetT>* functor = new cefix::Functor1ParamT<void, AbstractWidget::EventTypes, TextButtonWidgetT>(this, &TextButtonWidgetClass::handleMouseEvent);
			_drawImplementation->setCallback(Functor::create(this, &TextButtonWidgetT::handleMouseEvent));
		}
		
		/** ctor, creating from xml, see https://wiki.cefix.org/doku.php?id=widgetfactory */
		TextButtonWidgetT(cefix::PropertyList* pl) : 
			TextButtonWidgetClass(pl->get("id")->asString(), pl->get("position")->asVec3(), pl->get("size")->asVec2(), pl->get("caption")->asString())

		{
			if (pl->hasKey("selectable"))
				this->setSelectableFlag(pl->get("selectable")->asInt() != 0);
			
			if (pl->hasKey("selected")) {
				TextButtonWidgetClass::_selected = (pl->get("selected")->asInt() != 0);
			}	
			
			_drawImplementation = new DrawImplementationClass(pl->get("position")->asVec3(), pl->get("size")->asVec2());
			_drawImplementation->setCallback(Functor::create(this, &TextButtonWidgetT::handleMouseEvent));
			_drawImplementation->applyPropertyList(pl);
			
			
			
		}
		
		/** update the button */
		virtual void update() {
			_drawImplementation->update(this);
		}
		
		/** get the drawimplementation */
		DrawImplementationClass* getDrawImplementation() { return _drawImplementation.get(); }
		
		/** get the node */
		virtual osg::Node* getNode() { return _drawImplementation.get(); }
		
		/** handle the mouse-action */
		inline void handleMouseEvent(AbstractWidget::EventTypes action) 
		{
			TextButtonWidgetClass::handleMouseEvent(action);
		}
				
	protected:
		virtual ~TextButtonWidgetT() {
			_drawImplementation->setCallback(NULL);
		}
	private:
		osg::ref_ptr<DrawImplementationClass> _drawImplementation;

};


typedef TextButtonWidgetT<TextButtonWidget, SimpleTextButtonDrawImplementation> SimpleTextButtonWidget;


}	

	

#endif