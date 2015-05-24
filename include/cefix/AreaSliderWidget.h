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

#ifndef AREA_SLIDER_WIDGET_HEADER
#define AREA_SLIDER_WIDGET_HEADER

#include <cefix/SliderWidget.h>

namespace cefix {

class CEFIX_EXPORT AreaSliderWidget : public SliderWidget, public AbstractWidget::Responder {

	public:
        typedef osg::Vec2 value_type;
        
		AreaSliderWidget(const std::string& identifier, const osg::Vec3& pos, const osg::Vec2& size, const osg::Vec2& throbberSize, AbstractWidget::Responder* responder) :
			SliderWidget(identifier, pos, size, throbberSize, responder) {}
		
		AreaSliderWidget(cefix::PropertyList* pl) : SliderWidget(pl) {
			setRange( cefix::doubleRange(pl->get("hmin")->asDouble(), pl->get("hmax")->asDouble()), cefix::doubleRange(pl->get("vmin")->asDouble(), pl->get("vmax")->asDouble()) );
			setValue(pl->get("value")->asVec2());
		}
		
		virtual void respondToAction(const std::string& action, AbstractWidget* widget);
			
			
}; 

typedef SliderWidgetT<AreaSliderWidget, SimpleSliderDrawImplementation> SimpleAreaSliderWidget;


}

#endif