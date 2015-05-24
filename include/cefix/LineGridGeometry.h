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

#ifndef LINE_GRID_GEOMETRY_HEADER_
#define LINE_GRID_GEOMETRY_HEADER_

#include <osg/Geometry>
#include <osg/Geode>
#include <cefix/Export.h>
namespace cefix {

/** this class constructs a 2D/3D grid of lines, it hlights every 5th line in a different color */
class CEFIX_EXPORT LineGridGeometry : public osg::Geometry {

	public:
		/** ctor
		 *  @param lineCounts num lines for every dimension
		 *  @param unitSize stepsize for the different axes
		 *  @param hilighcolor color for every 5th line
		 *  @param color color for other lines */
		LineGridGeometry(const osg::Vec3& lineCounts, const osg::Vec3& unitSize = osg::Vec3(1,1,1), const osg::Vec4& hilightcolor = osg::Vec4(0,0.5f,0, 0.7f), const osg::Vec4& color = osg::Vec4(0,0.5f,0, 0.2f));
		
};


}

#endif