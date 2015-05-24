/*
 *  RadioButtonGroupWidget.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 20.11.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "RadioButtonGroupWidget.h"
#include "CheckboxWidget.h"
#include "WidgetFactory.h"
namespace cefix {

RadioButtonGroupWidget::RadioButtonGroupWidget(cefix::PropertyList* pl) :	
	GroupWidget(pl)
	
{
}



void RadioButtonGroupWidget::setNewSelection(CheckboxWidget* widget)
 {		
	_currentSelection = widget;
	for(WidgetMap::iterator i = _map.begin(); i != _map.end(); ++i) 
	{
		CheckboxWidget* cb = dynamic_cast<CheckboxWidget*>(i->second.get());
		if (cb && cb != widget) {
			cb->clearSelectionFlag();
			
			cb->update();
		}
	}
	 informAttachedResponder( Actions::radioButtonChanged() );
}



}



