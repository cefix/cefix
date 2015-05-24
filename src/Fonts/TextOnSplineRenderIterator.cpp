/*
 *  TextOnsplineRenderer.cpp
 *  NativeContext
 *
 *  Created by Stephan Huber on Mon Apr 07 2003.
 *  Copyright (c) 2003 __MyCompanyName__. All rights reserved.
 *
 */
#include <cefix/Log.h>
#include <osg/Matrix>
#include <osg/Quat>

#include <cefix/MathUtils.h>
#include "TextOnSplineRenderIterator.h"

namespace cefix {
namespace TextOnSplineResolvers {


// ----------------------------------------------------------------------------------------------------------
// findSegment
// ----------------------------------------------------------------------------------------------------------

bool SimpleLineStrip::findSegment(double posOnSpline, unsigned int& ndx, osg::Vec3& a, osg::Vec3& b) 
{
	bool finished = false;
	while ((ndx < _linestripSize-1) && (!finished)) 
	{
		a = (*_linestrip)[ndx];
		b = (*_linestrip)[ndx+1];
		
		osg::Vec3 delta(b-a);
		
		if ((posOnSpline >= (_lastDistance)) && (posOnSpline <= _lastDistance + delta.length() ))
			finished = 1;
		else {
			ndx++;
			_lastDistance += delta.length();
		}
	}
	
	return finished;
}


// ----------------------------------------------------------------------------------------------------------
// operator()
// ----------------------------------------------------------------------------------------------------------

bool SimpleLineStrip::operator()(double distanceOnSpline, osg::Vec3& pointOnSpline, osg::Vec3& upVector) {

	if (distanceOnSpline < _lastDistance) {
		_lastDistance = 0;
		_lastNdx = 0;
	}
	osg::Vec3 left, right;
	if (!findSegment(distanceOnSpline, _lastNdx, left, right)) 
		return false;
		
	double scalar = (distanceOnSpline - _lastDistance) / (right-left).length();
	
	pointOnSpline = cefix::interpolate(left, right, scalar);
	upVector = _upVector;
	
	return true;
}


// ----------------------------------------------------------------------------------------------------------
// operator()
// ----------------------------------------------------------------------------------------------------------

bool LineStrip::operator()(double distanceOnSpline, osg::Vec3& pointOnSpline, osg::Vec3& upVector) {

	if (distanceOnSpline < _lastDistance) {
		_lastDistance = 0;
		_lastNdx = 0;
	}
	osg::Vec3 left, right;
	if (!findSegment(distanceOnSpline, _lastNdx, left, right)) 
		return false;
		
	double scalar = (distanceOnSpline - _lastDistance) / (right-left).length();
	
	pointOnSpline = cefix::interpolate(left, right, scalar);
	upVector = cefix::interpolate( (*_upVectors)[_lastNdx], (*_upVectors)[_lastNdx+1], scalar);
	
	return true;
}


}
    

}