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

#ifndef CURVE_HEADER
#define CURVE_HEADER

#include <osg/Geometry>

#include <cefix/ControlPoint.h>
#include <cefix/Segment.h>

namespace cefix { 
namespace spline {
	/** base class for various spline-curves */
	class CEFIX_EXPORT Curve : public osg::Referenced {
		
		public:
			/** ctor
			  * @param controlpointlist a list of ControlPoints which defines the behavior of this curve 
			*/
			Curve(ControlPoint::List* controlpointlist);
			
						
			/* dtor */
			virtual ~Curve() {};
			
			/** gets the world position for a specific distance */
			
			inline void getWorldPosition(osg::Vec3& result, double distance) {
				unsigned int segment(0);
				getWorldPosition(result, distance, segment); 
			}
			
			virtual void getWorldPosition(osg::Vec3& result, double distance, unsigned int& segment) = 0;
			
			/** gets the position an a specifc cegment and distance */
			virtual void getPositionOnSegment(osg::Vec3& position,  double distance, unsigned int segment) = 0;
			
			/** gets the tangent for a specific distance */
			virtual void getTangent(osg::Vec3& tangent, double distance) = 0;
			
			/** gets the tangent for a specific distance on a specific segment */
			virtual void getTangentOnSegment(osg::Vec3& tangent, double distance, unsigned int segment) = 0;
			
			/** gets the world position for a specific distance */
			inline osg::Vec3 getWorldPosition(double distance) {
				osg::Vec3 result;
				getWorldPosition(result, distance);
				return result;
			}
			
			/** gets the position an a specifc cegment and distance */
			inline osg::Vec3 getPositionOnSegment(double distance, unsigned int segment) {
				osg::Vec3 result;
				getPositionOnSegment(result, distance, segment);
				return result;
			}
			
			/** gets the tangent for a specific distance */
			inline osg::Vec3 getTangent(double distance) {
				osg::Vec3 result;
				getTangent(result, distance);
				return result;
			}
			
			/** gets the tangent for a specific distance on a specific segment */
			inline osg::Vec3 getTangentOnSegment(double distance, unsigned int segment) {
				osg::Vec3 result;
				getTangentOnSegment(result, distance, segment);
				return result;
			}
			
			/** calculate the length of this curve */
			void calculateLength(float resolution);
			
			/** @return the length of this curve */
			inline double getLength() const  { return _length; }
			
			/** get the amount of countrol points */
			inline unsigned int getNumControlPoints() const { return (_segments.valid()) ? _segments->size()+1 : 0; }
			
			
			/** get a specific segment */
			inline const Segment& getSegment(unsigned int segment) const { return (*_segments)[segment]; }
			
			/** get a specific segment */
			inline Segment& getSegment(unsigned int segment) { return (*_segments)[segment]; }
			
			/** create a debug geometry visualizing the controlpoints and the tangents */
			osg::Geometry* createDebugGeometry() {
				if(!_debugGeometry.valid()) {
					_debugGeometry = new osg::Geometry();
					updateDebugGeometry();
				}
				return _debugGeometry.get();
			}
				
			/** sets the list of points defining the curve */
			void setPoints(ControlPoint::List* list);
			
			
			/** update the curve, compute the length and update a possible debuggeometry */
			inline void update(float resolution = 1000)
			{
				updateImplementation(); 
				calculateLength(resolution);
				if (_debugGeometry.valid()) {
					updateDebugGeometry();
				}
			}
			
		protected:
			/** should be implemented by inherited classes */
			virtual void updateImplementation() {};
			
			/** update the debug geometry */
			void updateDebugGeometry();
			
		protected:
			osg::ref_ptr<Segment::List>	_segments;
			osg::ref_ptr<osg::Geometry> _debugGeometry;
			double _length;
				
	
	};

}}

#endif