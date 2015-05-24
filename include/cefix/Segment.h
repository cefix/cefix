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
 
#ifndef SEGMENT_HEADER
#define SEGMENT_HEADER

#include <cefix/ControlPoint.h>
#include <cefix/Export.h>


namespace cefix { 
namespace spline {
	/** a curve-segment, it has two controlpoints as member */
	class Segment {
	
		public:
			/** a list of Segments */
			class List: public std::vector<Segment>, public osg::Referenced {
			
				public:
					virtual ~List() {}
			};
			
			/** ctor
			 * @param b begin of the segment 
			 * @param e end of the segment 
			 */
			Segment(const ControlPoint& b, const ControlPoint& e) : _begin(b), _end(e), _length(0), _lengthFromBeginning(0) {}
			
			/** ctor */
			Segment() :_length(0), _lengthFromBeginning(0) {}
			
			/** dtor */
			~Segment() {}
			
			/** sets the beginning */
			void setBegin(const ControlPoint& p) { _begin = p; }
			
			/** sets the end */
			void setEnd(const ControlPoint& p) { _end = p; }
			
			/** set the tangent of the beginning */
			void setBeginTangent(const osg::Vec3& t) {_begin.tangent = t; }
			
			/** set the tangent of the ending */
			void setEndTangent(const osg::Vec3& t) { _end.tangent = t; }
			
			/** sets the length */
			inline void setLength(double length) { _length = length; }
			
			/** sets the length from the beginning of the curve */
			inline void setLengthFromBeginning(double l) { _lengthFromBeginning = l; }
			
			/** get the beginning controlpoint */
			const ControlPoint& getBegin() { return _begin; }
			
			/** get the ending controlpoint */
			const ControlPoint& getEnd() { return _end; }
			
			/** get the length */
			inline double getLength() { return _length; }
			
			/** get the length from the beginning of the curve */
			inline double getLengthFromBeginning() { return _lengthFromBeginning; }
			
		private:
			ControlPoint _begin, _end;
			double			_length, _lengthFromBeginning;
	
	};

}
}

#endif