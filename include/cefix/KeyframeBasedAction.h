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

#ifndef KEYFRAME_BASED_ACTION_HEADER
#define KEYFRAME_BASED_ACTION_HEADER

#include "Timeline.h"
#include <cefix/KeyframeInterpolator.h>
#include <cefix/Serializer.h>

namespace cefix {

template <class T> 
class KeyframeBasedAction: public cefix::Timeline::Action 
{
public:
	typedef typename cefix::KeyFrameInterpolator<T>::KeyframeMap KeyframeMap;
	KeyframeBasedAction() : cefix::Timeline::Action(0,0), _keyframes(), _keyframeSampler(cefix::Timeline::SamplerFactory::get("linear")) {}
	
	void readFrom(cefix::Serializer& serializer) 
	{
		cefix::Timeline::Action::readFrom(serializer);
		readFromSerializer(serializer, _keyframes);
	}
	
	void writeTo(cefix::Serializer& serializer) 
	{
		cefix::Timeline::Action::writeTo(serializer);
		writeToSerializer(serializer, _keyframes);
	}
	
	inline T sample(const T& from, const T& to, double sample_scalar) 
	{
		return cefix::interpolate(from, to, sample_scalar);
	}
	
	double getSampleScalar(T& from, T& to, double scalar) 
	{
		double sample_scalar;
		double t = getStartTime() + getDuration()*scalar; //getParentTimeline()->getCurrentTime();
		if (!_keyframes.get(t, from, to, sample_scalar)) {
			from = to = _keyframes.getKeyframes().begin()->second;
		}
		
		return (*_keyframeSampler)(sample_scalar);
	}
	
	T sample(double scalar) 
	{
		T from, to;
		double sample_scalar = getSampleScalar(from, to, scalar);
		
		return sample(from, to, sample_scalar);
	}
	
	void addKeyframe(double t, const T& data) { _keyframes.add(t, data); recalculateStartTimeAndDuration(); }
	
	KeyframeMap&  getKeyframes() { return _keyframes.getKeyframes(); }
	const KeyframeMap&  getKeyframes() const { return _keyframes.getKeyframes(); }
	
	virtual void recalculateStartTimeAndDuration() 
	{
		const typename cefix::KeyFrameInterpolator<T>::KeyframeMap& map = _keyframes.getKeyframes();
		typename cefix::KeyFrameInterpolator<T>::KeyframeMap::const_iterator itr;
		if (map.size() < 2) return;
		
		itr = map.begin();
		double starttime = itr->first;
		itr = map.end();
		itr--;
		double endtime =itr->first;
		
		setStartTimeAndDuration(starttime, endtime - starttime);
	}
	
	cefix::Timeline::SamplerFunc getKeyframeSampler() const { return _keyframeSampler; }
	void setKeyframeSampler(const std::string& name) { _keyframeSampler = cefix::Timeline::SamplerFactory::get(name); }

protected:
	typename cefix::KeyFrameInterpolator<T>	_keyframes;
	cefix::Timeline::SamplerFunc			_keyframeSampler;
};

}


#endif