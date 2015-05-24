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

#ifndef VERTICAL_SLIDER_WIDGET_HEADER
#define VERTICAL_SLIDER_WIDGET_HEADER

#include "SliderWidget.h"

namespace cefix {

class CEFIX_EXPORT VerticalSliderWidget : public SliderWidget, public AbstractWidget::Responder {

	public:
		typedef SliderWidget::ResponderT<VerticalSliderWidget>	Responder;
		typedef double value_type;
        
		VerticalSliderWidget(	const std::string& identifier, 
								const osg::Vec3& pos, 
								const osg::Vec2& size, 
								const osg::Vec2& throbberSize,
								AbstractWidget::Responder* responder = NULL) :
			SliderWidget(identifier, pos, size, throbberSize, responder),
			AbstractWidget::Responder() {}
		
		VerticalSliderWidget(cefix::PropertyList* pl);
		
		void setRange(const cefix::doubleRange& range) { SliderWidget::setRange(cefix::doubleRange(0,0), range); }
		double getValue() { return SliderWidget::getValue()[1]; }
		void setValue(double v, bool ignoreResponder = false) { SliderWidget::setValue(osg::Vec2(0,v), ignoreResponder); }
		
		virtual void respondToAction(const std::string& action, AbstractWidget* widget);
		
		void setAllowedValues(std::vector<double>& values) {
			_allowedValues.clear();
			_allowedValues.reserve(values.size());
			
			for(unsigned int i = 0; i < values.size(); ++i) {
				_allowedValues.push_back(osg::Vec2(0, values[i]));
			}
		}
};
	
typedef SliderWidgetT<VerticalSliderWidget, SimpleSliderDrawImplementation> SimpleVerticalSliderWidget;

}


#endif