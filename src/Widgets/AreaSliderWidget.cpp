/*
 *  AreaSliderWidget.cpp
 *  cefixWidgets
 *
 *  Created by Stephan Huber on 04.06.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "AreaSliderWidget.h"
#include "HorizontalSliderWidget.h"
#include "VerticalSliderWidget.h"
#include <cefix/WidgetFactory.h>

namespace cefix {

void AreaSliderWidget::respondToAction(const std::string& action, AbstractWidget* widget) {

	if (action == SliderWidget::Actions::valueChanged()) {
	
		osg::Vec2 v = getValue();
		{ 			
			HorizontalSliderWidget* s = dynamic_cast<HorizontalSliderWidget*>(widget);
			if (s) {
				v[0] = s->getValue();
				setValue(v);
				return;
			}
		}
		{ 			
			VerticalSliderWidget* s = dynamic_cast<VerticalSliderWidget*>(widget);
			if (s) {
				v[1] = s->getValue();
				setValue(v);
				return;
			}
		}
		
		{ 
			AreaSliderWidget* s = dynamic_cast<AreaSliderWidget*>(widget);
			if (s) {
				setValue(s->getValue());
				return;
			}
		}
		
			
			
	
	}
}


}

