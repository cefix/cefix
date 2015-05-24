/*
 *  HoizontalSliderWidget.cpp
 *  cefixWidgets
 *
 *  Created by Stephan Huber on 04.06.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "HorizontalSliderWidget.h"
#include "AreaSliderWidget.h"
#include <cefix/WidgetFactory.h>
#include <cefix/Serializer.h>

namespace cefix {

HorizontalSliderWidget::HorizontalSliderWidget(cefix::PropertyList* pl) 
:	SliderWidget(pl) 
{
	setRange( cefix::doubleRange(pl->get("min")->asDouble(), pl->get("max")->asDouble()) );
	
	if (pl->hasKey("allowedValues")) 
	{
		cefix::Serializer serializer(pl);
		std::vector<double> allowedValues;
		serializer >> "allowedValues" >> allowedValues;
		setAllowedValues(allowedValues);
	}
	
	setValue(pl->get("value")->asDouble());			
}



void HorizontalSliderWidget::respondToAction(const std::string& action, AbstractWidget* widget) 
{
	if (action == SliderWidget::Actions::valueChanged()) {
		{
			HorizontalSliderWidget* s = dynamic_cast<HorizontalSliderWidget*>(widget);
			if (s) {
				setValue(s->getValue());
				return;
			}
		}
		{
			AreaSliderWidget* s = dynamic_cast<AreaSliderWidget*>(widget);
			if (s) {
				setValue(s->getValue()[0]);
				return;
			}
		}	
	}
}

}



