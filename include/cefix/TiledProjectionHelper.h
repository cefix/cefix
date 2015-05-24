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

#ifndef CEFIX_TILED_PROJECTION_HELPER_HEADER
#define CEFIX_TILED_PROJECTION_HELPER_HEADER

#include <osg/Matrix>
#include <cefix/WindowRotation.h>

namespace cefix {
	
	
/** small helper class encapsulating the math to compute an offset matrix for perspective / ortho projection */	
class TiledProjectionHelper {
public:	
	/// TiledProjectionHelper, @param w width of area @ param h height of area
	TiledProjectionHelper(float w, float h) : _width(w), _height(h) {}
	
	
	/// get offset matrix for projection for rect ax/ay - aw/aw
	osg::Matrix getOffsetForProjection(float ax, float ay, float aw, float ah, bool flipy = false);	
	
	/// get offset matrix for projection adapted to the given windowRotation for rect ax/ay - aw/aw
	osg::Matrix getRotatedOffsetForProjection(float ax, float ay, float aw, float ah,  WindowRotation::Rotation rotation = WindowRotation::NONE,bool flipy = false);	
	
	/// get offset matrix for ortho2-projection adapted to the given windowRotation for rect ax/ay - aw/aw
	osg::Matrix getOffsetForOrtho(float ax, float ay, float aw, float ah, WindowRotation::Rotation rotation = WindowRotation::NONE);
	
	
	float getHeight() { return  _height; }
	float getWidth()	{ return _width; }

	
private:

	float _width, _height;
};

}


#endif
