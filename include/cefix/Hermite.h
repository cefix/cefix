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

#ifndef HERMITE_SPLINE_HEADER
#define HERMITE_SPLINE_HEADER

#include <cefix/Curve.h>
#include <cefix/Export.h>

namespace cefix { 
namespace spline {

/** a hermite spline, see <http://en.wikipedia.org/wiki/Cubic_Hermite_spline> for more infos */
class CEFIX_EXPORT Hermite : public Curve {
	
	public:
		/** ctor */
		Hermite(ControlPoint::List* list) : Curve(list) {};
				
		/** implementation of getWorldPosition */
		virtual void getWorldPosition(osg::Vec3& result, double distance, unsigned int& segment);
		
		/** implementation of getPositionOnSegment */
		virtual void getPositionOnSegment(osg::Vec3& position,  double distance, unsigned int segment);
		
		/** implementation of getTangent */
		virtual void getTangent(osg::Vec3& tangent, double distance);
		
		/** implementation of getTangentOnSegment */
		virtual void getTangentOnSegment(osg::Vec3& tangent, double distance, unsigned int segment);
        
        using Curve::getWorldPosition;

};


}
}

#endif