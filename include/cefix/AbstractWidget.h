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

#ifndef ABSTRACT_WIDGET_HEADER
#define ABSTRACT_WIDGET_HEADER

#include <osg/observer_ptr>
#include <osg/Node>
#include <osg/Referenced>
#include <cefix/Functor.h>
#include <cefix/Export.h>
#include <cefix/Timer.h>

namespace cefix {

class PropertyList;

/** base class for all widgets, implements notification-system and event-types. Every widget must have an unique identifier. */
class CEFIX_EXPORT AbstractWidget : public virtual osg::Referenced {

	public:
		/** mouse states */
		enum States { DISABLED, PRESSED, DEPRESSED, NORMAL, SELECTED, NORMAL_OVER, SELECTED_OVER, UNKNOWN_OVER };
		/** mouse actions */
		enum EventTypes { MOUSE_ENTER, MOUSE_LEAVE, MOUSE_DOWN, MOUSE_UP, MOUSE_UP_OUTSIDE };
		
		/** the actions-class hols all actions as static functions returning string */
		class Actions {
		protected:
			Actions() {}
		};
		
		template <class WIDGET_CLASS>
		class Implementation : public osg::Referenced {
			
			public:
				Implementation(WIDGET_CLASS* widget) : osg::Referenced(), _widget(widget) {}
				
				void clear() { _widget = NULL; }
				
				virtual void update() = 0;
				virtual osg::Node* getNode() = 0;
				virtual void applyPropertyList(cefix::PropertyList* pl) = 0;
				
				inline WIDGET_CLASS* getWidget() { return _widget; }
				
			protected:
				
			
			private:	
				WIDGET_CLASS* _widget;
		
		};
		/** a responder class responds to an widget-action, it reacts for example to an button-press, or a new slider-widget. responder are registered with the widgets. 
		You can register one responer with multiple widgets, or multiple responder with one widget, it's up to you */
		class Responder : public virtual osg::Referenced {
			
			public:
				/** this method is called by the widget, which is clicked/changed/whatever
				  @param action the type of action, see particular widget-implementation
				  @param widget the widget, which provokes the action */
				virtual void respondToAction(const std::string& action, AbstractWidget* widget) = 0;
				
				/** called from AbstractWidget, when this responder get added to the list of responders of the widget */
				 virtual void addedToWidget(AbstractWidget* widget) {};
			
		};
		
		/** this class bridges a functor to a responder, it creates an functor from the given arguments, and calls it in the 
			respondToAction-method
		*/
		template <class T>class FunctorResponderBridge : public Responder {
			public:
				 FunctorResponderBridge(T* t, void(T::*fpt)(const std::string& action, AbstractWidget* widget)) : Responder(), _t(t), _fpt(fpt) 
				 {
				 }
				 
				 virtual void respondToAction(const std::string& action, AbstractWidget* widget) {
					if (_t.valid())
						(*_t.*_fpt)(action, widget);
				 }
				 
			private:
				osg::observer_ptr<T>	_t;
				void(T::*_fpt)(const std::string& action, AbstractWidget* widget);
		};
		
		template <class T1, class T2> 
		class CompositeResponder : public T1, public T2 {
			
			public:
				virtual void respondToAction(const std::string& action, AbstractWidget* widget) 
				{
					T1::respondToAction(action, widget);
					T2::respondToAction(action, widget);
				}
		};
				 
		
		typedef std::vector< osg::ref_ptr< Responder > > ResponderVector;
		typedef std::vector< osg::observer_ptr< Responder > > ObservedResponderVector;
	protected:
		/** ctor */
		AbstractWidget(const std::string& identifier) : osg::Referenced(), _identifier(identifier), _enabled(true), _parent(NULL) {}
	
	public:
		/** adds an responder to the list of active responder */
		void addResponder(Responder* responder) { 
			if (dynamic_cast<AbstractWidget*>(responder))
				_observedResponder.push_back(responder);
			else
				_responder.push_back(responder); 
			
			responder->addedToWidget(this);
		}
		
		/** removes a responder */
		void removeResponder(Responder* responder);
		
		/** @return the identifier of this widget */
		inline const std::string& getIdentifier() const { return _identifier; }
		
		/** sets the identifier */
		void setIdentifier(const std::string& identifier) { _identifier = identifier; }
		
		/** gets the graphical representation */
		virtual osg::Node* getNode() = 0;
		
		inline bool isEnabled() { return (getParent()) ? getParent()->isEnabled() && _enabled : _enabled; }
		
		/** disable widget */
		inline void disable() { if (!_enabled) return; _enabled = false; enabledFlagChanged(); update(); }
		
		/** enable widget */
		inline void enable() { if (_enabled) return;  _enabled = true; enabledFlagChanged();  update(); }
		
        void setEnabled(bool f) { if (f) enable(); else disable(); }
		
		/** update graphical representation */
		virtual void update() = 0;
		
		// sets the parent */
		virtual void setParent(AbstractWidget* w) { _parent = w; }
		
		void addParent(AbstractWidget* w);
		
		void removeParent(AbstractWidget* w);
		
		/** get the parent */
		AbstractWidget* getParent() { return _parent; }
		
		/** helper method to create a FunctorResponderBridge, so you can add easily your class's methods as a responder to a widget */
		template<class T>  static FunctorResponderBridge<T>* createFunctorResponderBridge(T* t, void(T::*fpt)(const std::string& action, AbstractWidget*)) {
			osg::ref_ptr< FunctorResponderBridge<T> > frp = new FunctorResponderBridge<T>(t, fpt);
			return frp.release();
		}
		
		/** add a method/object pair as a responder to this widget */
		template<class T>  FunctorResponderBridge<T>* addResponder(T* t, void(T::*fpt)(const std::string& action, AbstractWidget*)) {
			osg::ref_ptr< FunctorResponderBridge<T> > frp = createFunctorResponderBridge<T>(t, fpt);
			addResponder(frp.get());
			return frp.release();
		}
		
		template<class WidgetClass, typename T>
		class ChangeReferenceResponder : public Responder {
		public:
			ChangeReferenceResponder(T* t)
			:	Responder(),
				_t(t)
			{
			}
			
			virtual void addedToWidget(AbstractWidget* widget) { 
				_w = dynamic_cast<WidgetClass*>(widget); 
				if (_t && _w.valid()) {
					cefix::Timer* t = cefix::FunctorTimer::create(0.25, this, &ChangeReferenceResponder::updateValue);
					t->start();
					_value =  (*_t);
					_ignoreWidgetResponse = true;
					_w->setValue(_value);
					_ignoreWidgetResponse = false;
				}
			}
			
			virtual void respondToAction(const std::string& action, AbstractWidget* widget) 
			{
				if (_ignoreWidgetResponse) return ;
				_setterActive = true;
				if (_t && _w.valid()) {
					T v(_w->getValue());
					if (v != _value) {
						_value = v;
						(*_t) = v;
					}
				}
				_setterActive = false;
			}
			
		protected:
			void updateValue(cefix::Timer* t) {
				if (_setterActive) return;
				
				if (_t && _w.valid()) {
					T v(*_t);
					if (v != _value) {
						_value= v;
						_w->setValue(v);
					}
				} else {
					t->stop();
				}
			}
		private:
			osg::observer_ptr<WidgetClass> _w;
			T* _t;
			T _value;
			bool _setterActive, _ignoreWidgetResponse;
		};
		
		template<class WidgetClass, class T, typename U>
		class GetterSetterResponder : public Responder {
		public:
			GetterSetterResponder(T* t, U(T::*getter)(), void(T::*setter)(U u))
			:	Responder(),
				_w(NULL),
				_t(t),
				_getter(getter),
				_const_getter(NULL),
				_setter(setter),
				_constSetter(NULL),
				_value(),
				_setterActive(false),
				_ignoreWidgetResponse(false)
			{
			}
			
			GetterSetterResponder(T* t, U(T::*getter)(), void(T::*setter)(const U& u))
			:	Responder(),
				_w(NULL),
				_t(t),
				_getter(getter),
				_const_getter(NULL),
				_setter(NULL),
				_constSetter(setter),
				_value(),
				_setterActive(false),
				_ignoreWidgetResponse(false)
			{
			}
			
			GetterSetterResponder(T* t, U(T::*getter)() const, void(T::*setter)(U u))
			:	Responder(),
				_w(NULL),
				_t(t),
				_getter(NULL),
				_const_getter(getter),
				_setter(setter),
				_constSetter(NULL),
				_value(),
				_setterActive(false),
				_ignoreWidgetResponse(false)
			{
			}
			
			GetterSetterResponder(T* t, U(T::*getter)() const, void(T::*setter)(const U& u))
			:	Responder(),
				_w(NULL),
				_t(t),
				_getter(NULL),
				_const_getter(getter),
				_setter(NULL),
				_constSetter(setter),
				_value(),
				_setterActive(false),
				_ignoreWidgetResponse(false)
			{
			}
			
			GetterSetterResponder(T* t, void(T::*setter)(const U& u))
			:	Responder(),
				_w(NULL),
				_t(t),
				_getter(NULL),
				_const_getter(NULL),
				_setter(NULL),
				_constSetter(setter),
				_value(),
				_setterActive(false),
				_ignoreWidgetResponse(false)
			{
			}
			
			GetterSetterResponder(T* t, void(T::*setter)(U u))
			:	Responder(),
				_w(NULL),
				_t(t),
				_getter(NULL),
				_const_getter(NULL),
				_setter(setter),
				_constSetter(NULL),
				_value(),
				_setterActive(false),
				_ignoreWidgetResponse(false)
			{
			}
			
			virtual void addedToWidget(AbstractWidget* widget) { 
				_w = dynamic_cast<WidgetClass*>(widget); 
				if (_w.valid() && (_getter || _const_getter)) {
					cefix::Timer* t = cefix::FunctorTimer::create(0.25, this, &GetterSetterResponder::updateValue);
					t->start();
					_value =  getValue();
					_ignoreWidgetResponse = true;
					_w->setValue(_value);
					_ignoreWidgetResponse = false;
				}
			}
			
			virtual void respondToAction(const std::string& action, AbstractWidget* widget) 
			{
				if (_ignoreWidgetResponse) return ;
				_setterActive = true;
				if (_w.valid() && _t.valid() && (_setter ||_constSetter)) {
					U v(_w->getValue());
					if (v != _value) {
						_value = v;
						(_setter) ? (*_t.*_setter)(_value) : (*_t.*_constSetter)(_value);
					}
				}
				_setterActive = false;
			}
			
		protected:
			void updateValue(cefix::Timer* t) {
				if (_setterActive) return;
				
				if (_w.valid() && _t.valid() && (_getter || _const_getter) ) {
					U v( getValue() );
					if (v != _value) {
						_value= v;
						_w->setValue(v);
					}
				} else {
					t->stop();
				}
			}
		
			inline U getValue() { return (_getter) ? (*_t.*_getter)() : (*_t.*_const_getter)(); }
		private:
			osg::observer_ptr<WidgetClass> _w;
			osg::observer_ptr<T> _t;
			U(T::*_getter)();
			U(T::*_const_getter)() const;
			void(T::*_setter)(U u);	
			void(T::*_constSetter)(const U& u);		
			U _value;
			bool _setterActive, _ignoreWidgetResponse;
		};
		
		
		/** creates a responder, which calls a setter method when the widget gets changed, und which updates the value of the widget in regular intervalls */
		template<class WidgetClass, class T, typename U>
		static GetterSetterResponder<WidgetClass, T, U>*  createResponderFromGetterSetter(T* t, U(T::*getter)(), void(T::*setter)(U u) ) 
		{
			osg::ref_ptr< GetterSetterResponder<WidgetClass, T, U> > r = new GetterSetterResponder<WidgetClass, T, U>(t, getter, setter);
			return r.release();
		}
		
		/** creates a responder, which calls a setter method when the widget gets changed, und which updates the value of the widget in regular intervalls */
		template<class WidgetClass, class T, typename U>
		static GetterSetterResponder<WidgetClass, T, U>*  createResponderFromGetterSetter(T* t, U(T::*getter)(), void(T::*setter)(const U& u) ) 
		{
			osg::ref_ptr< GetterSetterResponder<WidgetClass, T, U> > r = new GetterSetterResponder<WidgetClass, T, U>(t, getter, setter);
			return r.release();
		}
		
		/** creates a responder, which calls a setter method when the widget gets changed, und which updates the value of the widget in regular intervalls */
		template<class WidgetClass, class T, typename U>
		static GetterSetterResponder<WidgetClass, T, U>*  createResponderFromGetterSetter(T* t, U(T::*getter)() const, void(T::*setter)(U u) ) 
		{
			osg::ref_ptr< GetterSetterResponder<WidgetClass, T, U> > r = new GetterSetterResponder<WidgetClass, T, U>(t, getter, setter);
			return r.release();
		}
		
		/** creates a responder, which calls a setter method when the widget gets changed, und which updates the value of the widget in regular intervalls */
		template<class WidgetClass, class T, typename U>
		static GetterSetterResponder<WidgetClass, T, U>*  createResponderFromGetterSetter(T* t, U(T::*getter)() const, void(T::*setter)(const U& u) ) 
		{
			osg::ref_ptr< GetterSetterResponder<WidgetClass, T, U> > r = new GetterSetterResponder<WidgetClass, T, U>(t, getter, setter);
			return r.release();
		}
		
		/** creates a responder, which calls a setter method when the widget gets changed */
		template<class WidgetClass, class T, typename U>
		static GetterSetterResponder<WidgetClass, T, U>* createResponderFromSetter(T* t, void(T::*setter)(U u) ) 
		{
			U(T::*getter)() const = NULL;
            return createResponderFromGetterSetter<WidgetClass, T, U>(t, getter, setter);
		}
		
		/** creates a responder, which calls a setter method when the widget gets changed */
		template<class WidgetClass, class T, typename U>
		static GetterSetterResponder<WidgetClass, T, U>* createResponderFromSetter(T* t, void(T::*setter)(const U& u) ) 
		{
			U(T::*getter)() const = NULL;
            return createResponderFromGetterSetter<WidgetClass, T, U>(t, getter, setter);
		}
		
		/** creates a responder, which calls a setter method when the widget gets changed */
		template<class WidgetClass, class T>
		static ChangeReferenceResponder<WidgetClass, T>* createResponderFromValue(T* t) 
		{
			osg::ref_ptr< ChangeReferenceResponder<WidgetClass, T> > r = new ChangeReferenceResponder<WidgetClass, T>(t);
			return r.release();
		}
		
	protected:
		/// react on the change of the enabled-flag
		virtual void enabledFlagChanged() {}
		/** dtor*/
		virtual ~AbstractWidget() {
			//std::cout << "destructing widget " << getIdentifier() << std::endl;
		}
		
		/** inform attached responder of an action, onyl called, if a widget is enabled */
		void informAttachedResponder(const std::string& actionIdentifier);
		
	private:
		std::string		_identifier;
		ResponderVector	_responder;
		ObservedResponderVector	_observedResponder;
		bool			_enabled;
		AbstractWidget*	_parent;
		std::list<AbstractWidget*> _parents;
		
};

template <class T>
class RectControlWidgetT : public T {

	public:
		class Actions: public AbstractWidget::Actions {
		public:
			static const char* stateChanged() { return "state_changed"; }
			static const char* selectionChanged() { return "selection_changed"; }
		protected:
			Actions() {}
		
		};
		RectControlWidgetT(const std::string& identifier, const osg::Vec3& pos, const osg::Vec2& size) 
		:	T(identifier), 
			_position(pos), 
			_size(size),
			_selectable(false),
			_selected(false),
			_down(false),
			_state(AbstractWidget::NORMAL)
		{
		}
		
		inline const osg::Vec3& getPosition() const { return _position; }
		inline const osg::Vec2& getSize() const { return _size; }
		inline float getWidth() const { return _size[0]; }
		inline float getHeight() const { return _size[1]; }
		
		void setPosition(const osg::Vec3& p) { _position = p; doMove(); };
		void setSize(const osg::Vec2& s) { _size = s; doResize(); };
		
		void setPositionAndSize(const osg::Vec3& p, const osg::Vec2& s) { _position = p; _size = s; doMove(); doResize(); }
		
		/**@return true, if button is selected */
		bool isSelected() { return _selected; }
		
		/** sets the selected-state of this button, calls the responder regadrdless of the old state */
		virtual void setSelected(bool f) 
		{ 
			if (_selectable) {
				_selected = f; 
				checkState(); 
				this->informAttachedResponder( Actions::selectionChanged() );
			}
		}
		
		/** checks the state of the button in regards to the selected-state */
		void checkState() 
		{
			if (_selected && (this->getState() == AbstractWidget::NORMAL_OVER))
				this->setState(AbstractWidget::SELECTED_OVER);
			else if ((!_selectable || !_selected) && (this->getState() == AbstractWidget::SELECTED_OVER))
				this->setState(AbstractWidget::NORMAL_OVER);
			else if (_selected && (this->getState() == AbstractWidget::NORMAL))
				this->setState(AbstractWidget::SELECTED);
			else if ((!_selectable || !_selected) && (this->getState() == AbstractWidget::SELECTED))
				this->setState(AbstractWidget::NORMAL);
		}
		
		/** @return the current state of this button */
		inline AbstractWidget::States getState() { return this->isEnabled() ?  _state : AbstractWidget::DISABLED; }
		
		/** sets the state of this button */
		void setState(AbstractWidget::States state, bool forceUpdate = false) {

				
			if (_state != state || forceUpdate) {
				_state = state;
				this->update();
				this->informAttachedResponder( Actions::stateChanged() );
			}
		}
		/** handle mouse action, sets the button's state acordingly. This method must be called by the graphical representation */
		inline void handleMouseEvent(AbstractWidget::EventTypes action) 
		{
			switch(action) {
				case AbstractWidget::MOUSE_ENTER:
					if (!_down) this->setState( (this->isSelected()) ? AbstractWidget::SELECTED_OVER : AbstractWidget::NORMAL_OVER );
					break;
					
				case AbstractWidget::MOUSE_LEAVE:
					if (!_down) this->setState( (this->isSelected()) ? AbstractWidget::SELECTED : AbstractWidget::NORMAL );
					break;
					
				case AbstractWidget::MOUSE_DOWN:
					_down = true;
					this->setState(AbstractWidget::PRESSED);
					break;
					
				case AbstractWidget::MOUSE_UP:
					_down = false;
					if (_selectable && this->isEnabled() ) 
						this->setSelected(!_selected);
					this->setState(AbstractWidget::DEPRESSED);
					this->setState( (_selected) ? AbstractWidget::SELECTED_OVER : AbstractWidget::NORMAL_OVER);	
					break;
				
				case AbstractWidget::MOUSE_UP_OUTSIDE:
					_down = false;
					this->setState(AbstractWidget::DEPRESSED);
					this->setState( (_selected) ? AbstractWidget::SELECTED : AbstractWidget::NORMAL);	
					break;
			}
		}
		
		
	protected:
		virtual void doMove() { this->update(); }
		virtual void doResize() { this->update(); }
		
		osg::Vec3 _position;
		osg::Vec2 _size;
		bool	  _selectable, _selected, _down;
		AbstractWidget::States	  _state;
};


typedef RectControlWidgetT<AbstractWidget> RectControlWidget;

template <class RectControlWidgetClass, class T>
class WidgetStateFunctorResponder : public AbstractWidget::Responder {
public:
	WidgetStateFunctorResponder(cefix::AbstractWidget::States state, T* t, void (T::*fpt)() ) : AbstractWidget::Responder(), _state(state), _t(t), _fpt(fpt) {}
	virtual void respondToAction(const std::string& action, AbstractWidget* widget)
	{
		if (action == RectControlWidget::Actions::stateChanged())
		{
			RectControlWidgetClass* btn = dynamic_cast<RectControlWidgetClass*>(widget);
			if(btn && (btn->getState() == _state) && _t.valid()) {
				(*_t.*_fpt)();
			}
		}
	}
private:
	cefix::AbstractWidget::States _state;
	osg::observer_ptr<T> _t;
	void (T::*_fpt)();
};
	

} // end of namespace
#endif