/*
 *  MouseLocationProvider.cpp
 *  CommonTools
 *
 *  Created by Stephan Huber on 25.04.06.
 *  Copyright 2006 digitalmind.de // Stephan Maximilian Huber. All rights reserved.
 *
 */

#include <cefix/MouseLocationProvider.h>
#include <cefix/DisplayCollection.h>
#include <cefix/ApplicationWindow.h>

namespace cefix {

    WindowPickable* MouseLocationProvider::instance() {
        static osg::ref_ptr<WindowPickable> s_mouse_location_provider = NULL;
		if (!s_mouse_location_provider.valid()) 
		{
			cefix::Display* display = DisplayCollection::instance()->getDisplay("main");
			cefix::ApplicationWindow* win = (display) ? display->getWindow() : NULL;
			s_mouse_location_provider = (win) ? win->getWindowPickable() : NULL;
		}
        return s_mouse_location_provider.get();
}



}

