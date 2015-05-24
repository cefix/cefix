/*
 *  ThemedWidgetStateGeometry.cpp
 *  cefixcvTemplate
 *
 *  Created by Stephan Huber on 23.10.08.
 *  Copyright 2008 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "ThemedWidgetStateGeometry.h"

namespace cefix {
ThemedWidgetStateGeometry::ThemedWidgetStateGeometry()
:	Resizable9x9TexturedGeometry(osg::Vec4(0,0,0,0), 0, 0, 0),
	_lastState(UNKNOWN)
{
	getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
	getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
}


void ThemedWidgetStateGeometry::addMissingStates()
{
	if (hasState(SELECTED)) {
		if (!hasState(SELECTED_DOWN))
			_states[SELECTED_DOWN] = _states[SELECTED];
		if (!hasState(SELECTED_OVER))
			_states[SELECTED_OVER] = _states[SELECTED];
		if (!hasState(SELECTED_DISABLED))
			_states[SELECTED_DISABLED] = _states[SELECTED];
	}
	
	if (hasState(NORMAL_DOWN)) {
		if (!hasState(SELECTED_DOWN)) 
			_states[SELECTED_DOWN] = _states[NORMAL_DOWN];
		
	}
	
	if (hasState(NORMAL)) {
		if (!hasState(NORMAL_OVER))
			_states[NORMAL_OVER] = _states[NORMAL];
		
		if (!hasState(NORMAL_DISABLED))
			_states[NORMAL_DISABLED] = _states[NORMAL];
	}
}
}
