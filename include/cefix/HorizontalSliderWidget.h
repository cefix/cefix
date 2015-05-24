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

#ifndef HORIZONTAL_SLIDER_WIDGET_HEADER
#define HORIZONTAL_SLIDER_WIDGET_HEADER


#include <cefix/SliderWidget.h>

namespace cefix {

/** a horizontal slider widget */
class CEFIX_EXPORT HorizontalSliderWidget : public SliderWidget, public AbstractWidget::Responder {

	public:
		typedef SliderWidget::ResponderT<HorizontalSliderWidget>	Responder;
        typedef double value_type;
		
		/** ctor 
		    @param identifier identifier of this widget, 
		    @param pos the position of the widget, 
		    @param size the size of the widget
			@param throbberSize, the Size of the throbber */
		HorizontalSliderWidget(	const std::string& identifier, 
								const osg::Vec3& pos, 
								const osg::Vec2& size, 
								const osg::Vec2& throbberSize,
								AbstractWidget::Responder* responder = NULL) :
			SliderWidget(identifier, pos, size, throbberSize, responder),
			AbstractWidget::Responder() {}
		
		HorizontalSliderWidget(cefix::PropertyList* pl);		
		/** sets the range for this slider @param range the new range */
		void setRange(const cefix::doubleRange& range) { SliderWidget::setRange(range, cefix::doubleRange(0,0)); }
		
		/** @return the current value of this slider */
		double getValue() {
			return SliderWidget::getValue()[0]; 
		}
		
		/** sets the value of the slider */
		void setValue(double v, bool ignoreResponder = false) { SliderWidget::setValue(osg::Vec2(v,0), ignoreResponder); }
		
		/** implementation of AbstractWidget::Responder::respondToAction, this allows you to set a slider as a responder to another slider */
		virtual void respondToAction(const std::string& action, AbstractWidget* widget);
		
		void setAllowedValues(std::vector<double>& values) {
			_allowedValues.clear();
			_allowedValues.reserve(values.size());
			
			for(unsigned int i = 0; i < values.size(); ++i) {
				_allowedValues.push_back(osg::Vec2(values[i], 0));
			}
		}
};
	
/** Simple horizontal slider class which adapts SimpleSliderDrawImplementation for a HorizontalSliderWidget*/
typedef SliderWidgetT<HorizontalSliderWidget, SimpleSliderDrawImplementation> SimpleHorizontalSliderWidget;

}

#endif