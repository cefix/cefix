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

#ifndef SLIDER_WIDGET_HEADER
#define SLIDER_WIDGET_HEADER
#include <osg/Math>
#include <cefix/Functor.h>
#include <cefix/MathUtils.h>

#include <cefix/SimpleSliderDrawImplementation.h>

#include <cefix/AbstractWidget.h>
#include <cefix/PropertyList.h>

namespace cefix {


class SliderWidget: public RectControlWidget {
	public:
		class Actions: public RectControlWidget::Actions {
		public:
			static const char* valueChanged() { return "valuechanged"; }
		protected:
			Actions(): RectControlWidget::Actions() {}
		};
		
		template <class SliderClass>
		class ResponderT : public virtual AbstractWidget::Responder {
			public:
				ResponderT() : AbstractWidget::Responder() {}
				virtual void sliderValueChanged() {}
				
				virtual void respondToAction(const std::string& action, AbstractWidget* widget) 
				{
					SliderClass* _slider = dynamic_cast<SliderClass*>(widget);
					if (_slider && (action=="valuechanged")) 
						sliderValueChanged();
				}

			protected:
				SliderClass* getSlider() { return _slider; }
			private:
				SliderClass* _slider;
		};
		
		typedef ResponderT<SliderWidget>	Responder;

	protected:
		SliderWidget(const std::string& identifier, const osg::Vec3& p, const osg::Vec2& size, const osg::Vec2& throbberSize, cefix::AbstractWidget::Responder* responder = NULL) :
			RectControlWidget(identifier, p, size),
			_throbberSize(throbberSize),
			_horizontalRange(cefix::doubleRange(0, size[0])),
			_verticalRange(cefix::doubleRange(0, size[1])),
			_value(osg::Vec2(0,0)),
			_resolution(0)
		
		{	
			if (responder)
				addResponder(responder);
		}
		
		SliderWidget(cefix::PropertyList* pl) :
			RectControlWidget(
				pl->get("id")->asString(),
				pl->get("position")->asVec3(),
				pl->get("size")->asVec2()
			),
			_throbberSize(pl->get("throbberSize")->asVec2()),
			_resolution(0)
		{
			osg::Vec2 size = getSize();
			_horizontalRange = cefix::doubleRange(0, size[0]);
			_verticalRange = cefix::doubleRange(0, size[1]);
		}
		
	public:
		void setRange(const cefix::doubleRange& h, const cefix::doubleRange& v) {
			_horizontalRange = h;
			_verticalRange = v;
			constrain();
			update();
		}
		
		void setResolution(unsigned int res) { _resolution = res; }
		
		osg::Vec2 getValue() { 
			osg::Vec2 v(_value);
			if (_resolution != 0) {
				v[0] = osg::round(_resolution * _value[0]) / (float)(_resolution);
				v[1] = osg::round(_resolution * _value[1]) / (float)(_resolution);
			}
			return v; 
		}
		
		void setValue(const osg::Vec2& value, bool ignoreResponder = false) {
			_value = value;
			constrain();
			update();			
			
			if (!ignoreResponder) 
				informAttachedResponder( Actions::valueChanged() );
		}
		
		virtual void update() {}

		osg::Vec3 getThrobberPos() {
			return osg::Vec3(
				((_horizontalRange.max() - _horizontalRange.min()) > 0) ? (_value[0] - _horizontalRange.min()) / (_horizontalRange.max() - _horizontalRange.min()) * (_size[0] - _throbberSize[0]) : 0,
				((_verticalRange.max() - _verticalRange.min()) > 0) ? (_value[1] - _verticalRange.min()) / (_verticalRange.max() - _verticalRange.min()) * (_size[1] - _throbberSize[1]) : 0 ,
				_position[2] + 1
			);
		}
		
		void setAllowedValues(const std::vector<osg::Vec2>& values) { _allowedValues = values; }
		
	
		void dragCallback(const osg::Vec3& dragPoint) 
		{
			if (!isEnabled()) return;
			
			updateValueFromThrobberPos(dragPoint-osg::Vec3(_throbberSize[0]/2, _throbberSize[1]/2, 0));
			update();
		}
		
protected:		
		void updateValueFromThrobberPos(const osg::Vec3& p) {
			osg::Vec3 constrained_pos(
				osg::clampTo(p[0], 0.0f,  _size[0] - _throbberSize[0]),
				osg::clampTo(p[1], 0.0f,  _size[1] - _throbberSize[1]),
				_position[2] + 1
			);
			
			// normalisieren
			_value[0] = ((_size[0] - _throbberSize[0]) > 0) ? constrained_pos[0] / (_size[0] - _throbberSize[0]) : 0;
			_value[1] = ((_size[1] - _throbberSize[1]) > 0) ? constrained_pos[1] / (_size[1] - _throbberSize[1]) : 0;
			
			// auf den Wertebereich skalieren
			_value[0] = _horizontalRange.interpolate(_value[0]);
			_value[1] = _verticalRange.interpolate(_value[1]);
			
			setValue(_value);
		}
	protected:	
		void constrain() {
			_value[0] = osg::clampTo(static_cast<double>(_value[0]), _horizontalRange.min(), _horizontalRange.max());
			_value[1] = osg::clampTo(static_cast<double>(_value[1]), _verticalRange.min(), _verticalRange.max());
			findNearestValue();
		}
	
		void findNearestValue();
		
		osg::Vec2			_throbberSize;
		cefix::doubleRange	_horizontalRange, _verticalRange;
		osg::Vec2			_value;
		unsigned int		_resolution;
		std::vector<osg::Vec2> _allowedValues;

};


class SliderDraggable : public Draggable {
	
	public: 
		SliderDraggable() : Draggable(), _slider(NULL), _origin() { stopPropagation(); }
		
		virtual void enter();
		
		virtual void leave();
		
		virtual void down();
		
		virtual void up(bool inside);
		
		virtual bool dragStarted();
		
		virtual bool drag(const osg::Vec3& delta);
		
		inline void setSlider(SliderWidget* slider) { _slider = slider; }
		inline void setOrigin(const osg::Vec3& o) { _origin = o; }
		
	private:
		osg::observer_ptr<SliderWidget> _slider;
		osg::Vec3 _origin, _p;
		float _divisor;
		osg::ref_ptr<const osgGA::GUIEventAdapter> _startEvent;
		
};




template <class SliderWidgetClass, class DrawImplementationClass = SimpleSliderDrawImplementation> class SliderWidgetT : public SliderWidgetClass {
	public:
		SliderWidgetT(const std::string& identifier, const osg::Vec3& pos, const osg::Vec2& size, const osg::Vec2& throbberSize, AbstractWidget::Responder* responder = NULL) :
			SliderWidgetClass(identifier, pos, size, throbberSize, responder),
			_firstRun(true)
		{	
			_drawImplementation = new DrawImplementationClass(pos, size, throbberSize);
			
			
			_drawImplementation->setSlider(this);
			_drawImplementation->update(this);
			
		}
		
		SliderWidgetT(cefix::PropertyList* pl) :
			SliderWidgetClass(pl),
			_firstRun(true)
		{	
			osg::Vec3 pos  = pl->get("position")->asVec3();
			osg::Vec2 size = pl->get("size")->asVec2();
			osg::Vec2 throbberSize = pl->get("throbberSize")->asVec2();
			
			if (pl->hasKey("resolution"))
				this->setResolution(pl->get("resolution")->asInt());
				
			_drawImplementation = new DrawImplementationClass(pos, size, throbberSize);
			
			//Functor1ParamT<void, const osg::Vec3&, SliderWidgetT>* functor = new Functor1ParamT<void, const osg::Vec3&, SliderWidgetT>(this, &SliderWidgetT::dragCallback);
			
			_drawImplementation->setSlider(this);
			_drawImplementation->update(this);
			_drawImplementation->applyPropertyList(pl);
		}


		
		DrawImplementationClass* getDrawImplementation() { return _drawImplementation.get(); }
		
		
		
		virtual osg::Node* getNode() { return _drawImplementation.get(); }
		
		virtual void update() {
			if (_firstRun) {
				_firstRun = false;
				SliderWidgetClass::setValue(SliderWidgetClass::getValue());
			}
			_drawImplementation->update(this);
		}
	protected:
		virtual ~SliderWidgetT() {
			_drawImplementation->setSlider(NULL);
		}
	private:
		osg::ref_ptr<DrawImplementationClass>		_drawImplementation;
		bool										_firstRun;
};




} // end of namespace

#endif
