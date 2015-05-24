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

#ifndef COLOR_UTILS_HEADER
#define COLOR_UTILS_HEADER

#include <osg/Vec4>
#include <osg/Vec4d>
#include <osg/Vec4f>

namespace cefix {
	/** converts a HSV-color to RGB, (alpha is set to 1) */
	osg::Vec4 HSVtoRGB(float h, float s, float v) ;  

	/** converts an rgb-color back to HSV, disregarding the alpha */
	void RGBtoHSV(osg::Vec4 rgb, float &h, float &s, float &v);
	
	/** get the grey value of a given color */
	inline float getGreyValue(const osg::Vec4f& c) { return 0.3f * c[0] + 0.59f * c[1] + 0.11f*c[2]; }
	
	inline double getGreyValue(const osg::Vec4d& c) { return 0.3 * c[0] + 0.59 * c[1] + 0.11 * c[2]; }
	
	/** converts a color to its grey value */
	inline osg::Vec4 toGrey(const osg::Vec4& c) { osg::Vec4::value_type grey = getGreyValue(c); return osg::Vec4(grey, grey, grey, c[3]); } 
	
	inline osg::Vec4 rgb(unsigned int rgb) 
	{ 
		return osg::Vec4( 
			((rgb & 0xFF0000) >> 16) / 255.0f, 
			((rgb & 0x00FF00) >> 8) / 255.0f, 
			 (rgb & 0x0000FF) / 255.0f, 1.0f); 
	}
	
	inline osg::Vec4 rgba(unsigned int rgba) 
	{ 
		return osg::Vec4(
			((rgba & 0xFF000000) >> 24) / 255.0f, 
			((rgba & 0x00FF0000) >> 16) / 255.0f, 
			((rgba & 0x0000FF00) >>  8) / 255.0f,
			 (rgba & 0x000000FF)        / 255.0f); 
	}
	
	
}

#endif