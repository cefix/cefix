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
 
#ifndef CONTROLPOINT_HEADER
#define CONTROLPOINT_HEADER

#include <vector>
#include <osg/Referenced>
#include <osg/Vec3>
#include <osg/Array>

namespace cefix { 
namespace spline {
	/** a cless defining a controlpoint of a curve, it has a position and a tangent */
	class ControlPoint {
				
		public:
			/** a list of controlPoints */
			class List : public std::vector<ControlPoint>, public osg::Referenced {
				public:
					List() : std::vector<ControlPoint>(), osg::Referenced() {}
					List(osg::Vec3Array* points, osg::Vec3Array* normals = NULL) : std::vector<ControlPoint>(), osg::Referenced() {
						for(unsigned int i = 0; i < points->size(); ++i) {
							push_back(ControlPoint( (*points)[i], (normals) ? (*normals)[i] : osg::Vec3(0,0,0)));
						}
					}
					
					template<class T>List(const T& bitr, const T& eitr) {
						T itr(bitr);
						while(itr != eitr) {
							push_back(ControlPoint( (*itr), osg::Vec3(0,0,0)) );
							++itr;
						}
					}
					
					virtual ~List() { clear(); }
			};
			/** ctor */
			ControlPoint() : position(), tangent() {}
			
			/** ctor */
			ControlPoint(const osg::Vec3& p, const osg::Vec3& t) : position(p), tangent(t) {}
			
			/** ctor */
			ControlPoint(const ControlPoint& rhs) : position(rhs.position), tangent(rhs.tangent) {}
			

		
			osg::Vec3 position, tangent; 
	};

}}

#endif