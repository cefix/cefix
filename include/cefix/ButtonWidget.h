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

#ifndef BUTTON_WIDGET_HEADER
#define BUTTON_WIDGET_HEADER


#include <cefix/AbstractWidget.h>
#include <cefix/SimpleButtonDrawImplementation.h>
#include <cefix/PropertyList.h>
#include <cefix/Functor.h>

namespace cefix {

/** a basic button widget, it can be switched to selected mode, so the button toggles between selected and not selected. 
Note: this widget has no graphical implementation, this is done with the help of the adapter-template ButtonWidgetT

The button sends the following actions: selectionchanged, statechanged  */
class ButtonWidget : public RectControlWidget {

	/** specialized Rsponderclass handling common events of a button: pressed, released and toggled ( if the button is selectable)
	    subclasses should implement the virtual functions buttonPressed, buttonReleased or buttonToggled
	*/
	public: 
        typedef bool value_type;
        
	class Actions : public RectControlWidget::Actions {
	
	private:
		Actions() : RectControlWidget::Actions() {}
	};
	class Responder : public virtual AbstractWidget::Responder {
	
		public:
			/** ctor */
			Responder() : AbstractWidget::Responder() {}
			
			/** the button was pressed */
			virtual void buttonPressed() {}
			
			/** the button was released */
			virtual void buttonReleased() {}
			
			/** the button was toggled, only called, if the buttoon is selectable */
			virtual void buttonToggled() {}

			/** do the hard work */
			virtual void respondToAction(const std::string& action, AbstractWidget* widget)
			{
				_btn = dynamic_cast<ButtonWidget*>(widget);
				if(_btn) {
					if (action == Actions::selectionChanged() )
						buttonToggled();
					else if (action == Actions::stateChanged() ) {
						switch (_btn->getState()) {
							case PRESSED:
								buttonPressed();
								break;
								
							case DEPRESSED:
								buttonReleased();
								break;
								
							default:
								break;
						}
					}
				}
			}
			
		protected:
			ButtonWidget* getButton() { return _btn; }
		private:
			ButtonWidget*	_btn;
	};
	template <class T>
	class FunctorResponder : public WidgetStateFunctorResponder<ButtonWidget, T> {
	public:
		FunctorResponder(cefix::AbstractWidget::States state, T* t, void (T::*fpt)() )
		:	WidgetStateFunctorResponder<ButtonWidget, T>(state, t, fpt)
		{
		}
	};

	protected: 
		/** ctor 
		    @param identifier identifier of this widget, 
		    @param pos the position of the widget, 
		    @param size the size of the widget
		*/
		ButtonWidget(const std::string& identifier, const osg::Vec3& pos, const osg::Vec2& size, AbstractWidget::Responder* responder = NULL) :
			RectControlWidget(identifier, pos, size)
		{
			if (responder) addResponder(responder);
		}
		
	public:
		template <class T> static  FunctorResponder<T>* createResponder(AbstractWidget::States state, T* t,  void (T::*fpt)() )
		{
			return new FunctorResponder<T>(state, t, fpt);
		}
		
		/** if set to true, the button toggles on mousedwon between selected and not selected */
		void setSelectableFlag(bool flag) { 
			_selectable = flag;  
			checkState();
		}
		
		inline bool getValue() { return isSelected(); }
		
		inline void setValue(bool f) { setSelected(f); }
		
		
							
	protected:
		virtual void enabledFlagChanged() { informAttachedResponder( Actions::stateChanged() ); }

		

};

/** template adapter class which combines a DrawImplementation-class with a ButtonWidgetClass. 
    The DrawImplementation-Class should "paint" the graphical representation of the button, and inform the button-logic about mouse-events via a functor. 
*/
template< class ButtonWidgetClass, class DrawImplementationClass = SimpleButtonDrawImplementation> class ButtonWidgetT : public ButtonWidgetClass {

	public:
		/** ctor, creates a new DrawImplementationClass and sets the callback 
		    @param identifier identifier of this widget, 
		    @param pos the position of the widget, 
		    @param size the size of the widget
		*/
		ButtonWidgetT(const std::string& identifier, const osg::Vec3& pos, const osg::Vec2& size, AbstractWidget::Responder* responder = NULL) : ButtonWidgetClass(identifier, pos, size, responder)
		{
			_drawImplementation = new DrawImplementationClass(pos, size);
			//cefix::Functor1ParamT<void, AbstractWidget::EventTypes, ButtonWidgetT>* functor = new cefix::Functor1ParamT<void, AbstractWidget::EventTypes, ButtonWidgetT>(this, &ButtonWidgetClass::handleMouseAction);
			_drawImplementation->setCallback(Functor::create(this,  &ButtonWidgetT::handleMouseEvent));
		}
		
		ButtonWidgetT(cefix::PropertyList* pl)  : 
			ButtonWidgetClass(pl->get("id")->asString(), pl->get("position")->asVec3(), pl->get("size")->asVec2())
		{
			if (pl->hasKey("selectable"))
				this->setSelectableFlag(pl->get("selectable")->asInt() != 0);
			if (pl->hasKey("selected")) {
				ButtonWidgetClass::_selected = (pl->get("selected")->asInt() != 0);
			}	
				
			_drawImplementation = new DrawImplementationClass(pl->get("position")->asVec3(), pl->get("size")->asVec2());
			_drawImplementation->setCallback(Functor::create(this, &ButtonWidgetT::handleMouseEvent));
			_drawImplementation->applyPropertyList(pl);
			
		}
		
		/** update the graphical representation */
		virtual void update() {
			_drawImplementation->update( this );
		}
		
		/** return the graphical representation for the scenegraph */
		virtual osg::Node* getNode() { return _drawImplementation.get(); }
		
		DrawImplementationClass* getDrawImplementation() { return _drawImplementation.get(); }
		
		inline void handleMouseEvent(AbstractWidget::EventTypes action) 
		{
			ButtonWidgetClass::handleMouseEvent(action);
		}
	
	protected:
		virtual ~ButtonWidgetT() {
			_drawImplementation->setCallback(NULL);
		}
		
	private:
		osg::ref_ptr<DrawImplementationClass> _drawImplementation;

};

/** Simple ButtonWidget, adapts SimpleButtonDrawImplementation with a ButtonWidget */
typedef ButtonWidgetT<ButtonWidget, SimpleButtonDrawImplementation> SimpleButtonWidget;


} // end of namespace

#endif