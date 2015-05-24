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

#ifndef CEFIX_KEY_FRAME_INTERPOLATOR_HEADER
#define CEFIX_KEY_FRAME_INTERPOLATOR_HEADER

#include <map>
#include <iostream>
#include <osg/Math>

namespace cefix {

/** template class to help to interpolate between a set of keyframes. 
    The keyframe-class is the template parameter, you can add keyframe-objects 
    with time-stamps and query later the list to get the two 
    keyframes + the scalar for a given time
 */
template <class KEYFRAME>
class KeyFrameInterpolator  {

public:
	typedef std::map<double, KEYFRAME> KeyframeMap;
	
	/// ctor
	KeyFrameInterpolator() {}
	~KeyFrameInterpolator() {}
	
	/// valid if 2 or more keyframes were added
	bool valid() const { return (_map.size() > 1); }
	
	/// add a keyframe object
	inline void add(double time, const KEYFRAME& keyframe) { 
		_map[ time ] = keyframe; 
		init();
	}
	
	void init() 
	{
		if (valid()) {
			_next = _map.begin();
			_current = _next++;
		}
	}
	
	
	/// get the left keyframe for a given time
	KEYFRAME& get(double time) 
	{			
		if ((time < (*_current).first) || (time >= (*_next).first)) {
			findNewCurrent(time);
		}
		return (*_current).second;
	}
	
	
	/// clear
	inline void clear() { _map.clear(); }
	
	
	/// get the two keyframes where time lies in and the scalar
	bool get(double time, KEYFRAME& keyframe_a, KEYFRAME& keyframe_b, double& scalar)
	{
		if (!valid())
			return false;
		
		if ((time < (*_current).first) || (time >= (*_next).first)) {
			findNewCurrent(time);
		}
		keyframe_a =(*_current).second;
		keyframe_b = (*_next).second; 
		
		double len = (*_next).first - (*_current).first;
		scalar = (time - (*_current).first) / len;
		//bool success = scalar >= 0 && scalar <= 1.0;
		scalar = osg::clampTo(scalar, 0.0,1.0);
		
		return true;
 
	}
	
	/// get the scalar for the keyframe  for time
	double getScalar(float time) 
	{
		if ((time < (*_current).first) || (time >= (*_next).first)) {
			findNewCurrent(time);
		}
		double len = (*_next).first - (*_current).first;
		return (time - (*_current).first) / len;
	}
	
	/// get the next keyframe for time 
	const KEYFRAME& getNext(double time) 
	{
		typename KeyframeMap::iterator c = get(time);
		++c;
		return (*c).second;
	}
	
	/// dump the keyframe-list
	void dump() {
		for(typename KeyframeMap::iterator i = _map.begin(); i != _map.end(); ++i) {
			std::cout << (*i).first;
			if (i == _current) std::cout << " isCurrent";
			if (i == _next) std::cout << " isNext";
			std::cout << std::endl;
		}
	}
	
	const KeyframeMap& getKeyframes() const { return _map; }
	KeyframeMap& getKeyframes()  { return _map; }
private:
	
	void findNewCurrent(float time)
	{
		if ((_current == _map.end()) || ( (*_current).first > time))
			_current = _map.begin();
	
		typename KeyframeMap::iterator c(_current), d(_current);
		while ((c != _map.end()) && ( (*c).first <= time )) {
			d = c; 
			++c;
		}
		if (c == _map.begin()) {
			_next = _current = c;
			_next++;
		} else {
			if (c == _map.end())
				_current = _next = d;
			else
				_current = _next = c;
			_current--;
		}
	}
	
	KeyframeMap _map;
	typename KeyframeMap::iterator _current, _next;
};

}

#endif