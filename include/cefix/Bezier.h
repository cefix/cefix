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

#ifndef BEZIER_SPLINE_HEADER
#define BEZIER_SPLINE_HEADER

#include <cefix/Curve.h>
#include <cefix/Export.h>

namespace cefix { 
namespace spline {

class CEFIX_EXPORT Bezier : public Curve {
	
	public:
		
		Bezier(ControlPoint::List* list);
		
		virtual void getWorldPosition(osg::Vec3& result, double distance, unsigned int& segment);
		virtual void getPositionOnSegment(osg::Vec3& position,  double distance, unsigned int segment);
		virtual void getTangent(osg::Vec3& tangent, double distance);
		virtual void getTangentOnSegment(osg::Vec3& tangent, double distance, unsigned int segment);
	

};

}
}

#endif
