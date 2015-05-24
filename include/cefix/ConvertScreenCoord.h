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


#ifndef CONVERT_2D_COORD_
#define CONVERT_2D_COORD_

#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Vec2>
#include <cefix/Export.h>

namespace cefix {

/** small template class to help in the conversion from screen-coords
 *   it converts top-left coords to bottom-left coords */
  
class Display;

class CEFIX_EXPORT ConvertScreenCoord {
    public:
        static float getHeight();
		static void setHeight(float height);
		static void applyDisplay(Display* display);
};

        
        template<typename T>T convertScreenCoord(T t) {
            float h = ConvertScreenCoord::getHeight();
            return h - t;
        }
        template<> osg::Vec4 CEFIX_EXPORT convertScreenCoord(osg::Vec4 r);        
        template<> osg::Vec3 CEFIX_EXPORT convertScreenCoord(osg::Vec3 r);

		template<typename T>T convertCoord(T t, float h) {
            return h - t;
        }

		template<>  osg::Vec4 CEFIX_EXPORT convertCoord(osg::Vec4 r, float h);

		template<>  osg::Vec3 CEFIX_EXPORT convertCoord(osg::Vec3 r, float h);
		
		template<>  osg::Vec2 CEFIX_EXPORT convertCoord(osg::Vec2 r, float h);




}
#endif