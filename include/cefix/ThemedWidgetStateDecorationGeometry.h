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

#ifndef THEMED_WIDGET_STATE_DECORATION_GEOMETRY_HEADER
#define THEMED_WIDGET_STATE_DECORATION_GEOMETRY_HEADER

#include <cefix/ThemedWidgetStateGeometry.h>

namespace cefix {

class ThemedWidgetStateDecorationGeometry : public ThemedWidgetStateGeometry {

	public:
		ThemedWidgetStateDecorationGeometry();
		
		void setInsets(float x, float y) { _insetWidth = x; _insetHeight = y; }
		
		virtual void setRect(const osg::Vec4& r, bool resetTextureRect = false);		
		virtual void setTextureRect(const osg::Vec4& r);
		
	private:
		float _insetWidth, _insetHeight,_imageWidth;

};

}


#endif