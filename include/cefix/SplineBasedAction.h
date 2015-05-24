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

#ifndef CEFIX_SPLINE_BASED_ACTION_HEADER
#define CEFIX_SPLINE_BASED_ACTION_HEADER

#include <cefix/Spline.h>
#include <cefix/Serializer.h>
#include "Timeline.h"

namespace cefix {


template <typename T>
struct SplineBasedActionTraits {
typedef T type;
};

template<>
struct SplineBasedActionTraits<float> {
typedef Spline1D type;
};

template<>
struct SplineBasedActionTraits<osg::Vec2> {
typedef Spline2D type;
};

template<>
struct SplineBasedActionTraits<osg::Vec3> {
typedef Spline3D type;
};


template <class T>
class SplineBasedAction : public cefix::Timeline::Action {

public:
	typedef typename SplineBasedActionTraits<T>::type spline_type;
	typedef typename spline_type::value_type value_type;
	
	SplineBasedAction() : cefix::Timeline::Action(0,0), _spline() {}
	
	void setMode(cefix::SplineMode mode) { _spline.setMode(mode); }
	void addControlPoint(const T& p) { _spline.controlPoints.push_back(p); }
	
	virtual void init() {
		cefix::Timeline::Action::init();
		_spline.buildArclenTable();
		_spline.normalizeArclenTable();
	}
	
	void readFrom(cefix::Serializer& serializer) 
	{
		cefix::Timeline::Action::readFrom(serializer);
		serializer >> "spline" >> _spline;
	}
	
	void writeTo(cefix::Serializer& serializer) 
	{
		cefix::Timeline::Action::writeTo(serializer);
		serializer << "spline" << _spline;
	}
	
	inline value_type sample(double scalar) 
	{
		return _spline.getApproximatedPointOnSplineByArcLen(scalar);
	}
    
    spline_type& getSpline() { return _spline; }
    const spline_type& getSpline() const { return _spline; }
	
private:
	
	spline_type _spline;

}; 

}



#endif