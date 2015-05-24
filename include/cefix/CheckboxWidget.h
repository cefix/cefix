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

#ifndef CHECK_BOX_WIDGET_HEADER
#define CHECK_BOX_WIDGET_HEADER

#include <cefix/TextButtonWidget.h>
#include <cefix/RadioButtonGroupWidget.h>
#include <cefix/SimpleCheckboxDrawImplementation.h>

namespace cefix {


/** simple checkbox widget which can be used as a radiobutton. If you want to use this widget as a radiobutton, be sure, to add the widget to a RadioButtonGroupWidget */
class CEFIX_EXPORT CheckboxWidget : public TextButtonWidget {

	protected:
		CheckboxWidget(const std::string& identifier, const osg::Vec3& pos, const osg::Vec2& size, const std::string& caption, Responder* responder = NULL);
		
	public:	
        typedef bool value_type;
		virtual void setSelected(bool f);
		
	protected:
		void clearSelectionFlag() {
			TextButtonWidget::setSelected(false);
		}
	friend class RadioButtonGroupWidget;
};

/** template adapter class which combines a DrawImplementation-class with a CheckboxWidgetClass. 
    The DrawImplementation-Class should "paint" the graphical representation of the button, and inform the button-logic about mouse-events via a functor. 
*/
template< class CheckboxWidgetClass, class DrawImplementationClass = SimpleCheckboxDrawImplementation> class CheckboxWidgetT : public CheckboxWidgetClass {

	public:
		/** ctor, creates a new DrawImplementationClass and sets the callback 
		    @param identifier identifier of this widget, 
		    @param pos the position of the widget, 
		    @param size the size of the widget
		*/
		CheckboxWidgetT(const std::string& identifier, const osg::Vec3& pos, const osg::Vec2& size, const std::string& caption, AbstractWidget::Responder* responder = NULL) : 
			CheckboxWidgetClass(identifier, pos, size, caption, responder),
			_firstRun(true)
		{
			_drawImplementation = new DrawImplementationClass(pos, size);
			_drawImplementation->setCallback(Functor::create(this,  &CheckboxWidgetT::handleMouseEvent));
		}
		
		CheckboxWidgetT(cefix::PropertyList* pl)  : 
			CheckboxWidgetClass(pl->get("id")->asString(), pl->get("position")->asVec3(), pl->get("size")->asVec2(), pl->get("caption")->asString()),
			_firstRun(true)
		{		
			
			_drawImplementation = new DrawImplementationClass(pl->get("position")->asVec3(), pl->get("size")->asVec2());
			_drawImplementation->setCallback(Functor::create(this, &CheckboxWidgetT::handleMouseEvent));
			_drawImplementation->applyPropertyList(pl);
			if (pl->hasKey("selected")) {
				CheckboxWidgetClass::_selected = (pl->get("selected")->asInt() != 0);
			}
		}
		
		/** update the graphical representation */
		virtual void update() {
			if (_firstRun) {
				_firstRun = false;
				CheckboxWidgetClass::setSelected(CheckboxWidgetClass::isSelected());
			}
			_drawImplementation->update(this);

		}
		
		/** return the graphical representation for the scenegraph */
		virtual osg::Node* getNode() { return _drawImplementation.get(); }
		
		DrawImplementationClass* getDrawImplementation() { return _drawImplementation.get(); }
		
		inline void handleMouseEvent(AbstractWidget::EventTypes action) 
		{
			CheckboxWidgetClass::handleMouseEvent(action);
		}
	
	protected:
		virtual ~CheckboxWidgetT() {
			_drawImplementation->setCallback(NULL);
		}
		
	private:
		osg::ref_ptr<DrawImplementationClass> _drawImplementation;
		bool									_firstRun;

};

/** Simple CheckboxWidget, adapts SimpleButtonDrawImplementation with a CheckboxWidget */
typedef CheckboxWidgetT<CheckboxWidget, SimpleCheckboxDrawImplementation> SimpleCheckboxWidget;
typedef CheckboxWidgetT<CheckboxWidget, SimpleCheckboxDrawImplementation> SimpleRadioButtonWidget;


}

#endif