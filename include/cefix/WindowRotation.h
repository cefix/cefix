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

#ifndef CEFIX_WINDOW_ROTATION_HEADER_
#define CEFIX_WINDOW_ROTATION_HEADER_

#include <string>
#include <osg/MatrixTransform>
#include <osg/Viewport>
#include <osg/ref_ptr>
#include <cefix/Export.h>
#include <cefix/EnumUtils.h>

namespace cefix {

class CEFIX_EXPORT WindowRotation {

	public:
		enum Rotation { UNKNOWN, NONE, TILTED_LEFT, UPSIDE_DOWN, TILTED_RIGHT};

		WindowRotation(Rotation rotation = NONE);
		WindowRotation(const std::string& prefskey);
				
		osg::Matrix computeProjectionMatrix(double hfov, double vfov, float nearplate, float farPlane);
		
		osg::Camera* createHudCamera(unsigned int w, unsigned int h, const osg::Matrix& offset = osg::Matrix());
		void setupHudCamera(osg::Camera* cam, unsigned int w, unsigned int h, const osg::Matrix& offset = osg::Matrix());
		
		osg::Viewport* getViewport() const {
			osg::ref_ptr<osg::Viewport> v;
			if (isTilted()) 
				v = new osg::Viewport(_top, _left, _height, _width);
			else
				v = new osg::Viewport(_left, _top, _width, _height);
			return v.release();
		}
		
		void setViewport(int left, int top, int width, int height) { _left = left; _top = top; _width = width; _height = height; }
		void setViewport(osg::Viewport* vp) { setViewport(vp->x(), vp->y(), vp->width(), vp->height()); }
		inline bool isTilted() const { return ((_rotation == TILTED_LEFT) || (_rotation == TILTED_RIGHT)); }
		
		inline int getWidth() const { return (isTilted()) ? _height : _width; }
		inline int getHeight() const { return (isTilted()) ? _width : _height; }
		
		inline int getLeft() const { return (isTilted()) ? _top : _left; }
		inline int getTop() const { return (isTilted()) ? _top : _left; }
		
		
		double getAngleForRotation();
		
		inline Rotation getRotation() const { return _rotation; }
		
		static Rotation readFromPrefs(const std::string prefsKey);
		
		static EnumAsString<Rotation, 5>& getRotationEnumsAsString();
	protected:
		
				
	private:
		Rotation _rotation;
		int _left, _top, _width, _height;

};


}



#endif