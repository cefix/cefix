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

#ifndef __BEZIER_CURVE_HEADER__
#define __BEZIER_CURVE_HEADER__

#include <osg/Vec3>
#include <osg/Array>
#include <cefix/Export.h>

using namespace osg;

namespace cefix {

	/**
	 * this class implements a Beziercurve 
	 *
	 * @author Stephan Maximilian Huber 
	 */
	 
	class CEFIX_EXPORT BezierCurve {
		protected:
			/** the start of the curve */
			Vec3 _start;
			/** the endpoint of the curve */
			Vec3 _end;
			/** the controlpoint for the start-point */
			Vec3 _controlStart;
			/** the controlpoint for the end-point */
			Vec3 _controlEnd;
			
			
		public:
			/**
			 * sets the start- and enpoint for this curve
			 * @param start the start-point
			 * @param end the end-point
			 */
			inline void setStartAndEndPoints(Vec3 start,Vec3 end) {_start = start; _end= end;}
			
			/**
			 * sets the control-handles for this curve
			 * @param start the controlpoint for the start-point
			 * @param end the controlpoint for the end-point
			 */
			inline void setControlHandles(Vec3 start,Vec3 end) {_controlStart = start; _controlEnd = end;}
			/**
			 * computes a osg::Vec3Array which describes the curve
			 * @param array this array gets appended by the new vertices
			 * @param steps steps steps points gets calculated, the higher the better the quality
			 * @param fAddFirstPoint if false, the first point is omitted
			 */
			void computeVecArray(Vec3Array* array,int steps, bool fAddFirstPoint);
			/**
			 * computes a osg::Vec3Array which describes the curve
			 * the computation is made by subdividing the curve, so the points are evenly spaced on the curve
			 * this method computes better curves, but is more expensive
			 * @param array this array gets appended by the new vertices
			 * @param maxDistance the maximimum distance between two points on the curve
			 * @param fAddFirstPoint if false, the first point is omitted
			 */
			void computeVecArrayBySubdivision(Vec3Array* array, float maxDistance, bool fAddFirstPoint);

			inline osg::Vec3 compute(float t) {
				osg::Vec3 result;
				float u(1-t);
				float b0 =       u * u * u;
				float b1 = 3.0 * t * u * u;
				float b2 = 3.0 * t * t * u;
				float b3 =       t * t * t;
				// Blend
				
				result = _start*b0 + _controlStart*b1 + _controlEnd*b2 + _end*b3;
				
				/*
				result[0] = b0 * _start.x() + b1 * _controlStart.x() + b2 * _controlEnd.x() + b3 * _end.x();
				result[0] = b0 * _start.y() + b1 * _controlStart.y() + b2 * _controlEnd.y() + b3 * _end.y();
				result[0] = b0 * _start.z() + b1 * _controlStart.z() + b2 * _controlEnd.z() + b3 * _end.z();
				*/
				return result;
			}
		
			virtual ~BezierCurve() {}

		protected:
			/**
			 * computes a osg::Vec3Array which describes the curve
			 * the computation is made by subdividing the curve, so the points are evenly spaced on the curve
			 * this method computes better curves, but is more expensive
			 * @param array this array gets appended by the new vertices
			 * @param start startpoint
			 * @param end endpoint
			 * @param handle1 first handle
			 * @param handle2 handle of endpoint
			 * @param maxDistance max distance between two points on the line
			 */
			void _computeVecArrayBySubDivision(Vec3Array* array, Vec3 start, Vec3 end, Vec3 handle1, Vec3 handle2, float maxDistance) ;
	};

}
#endif

