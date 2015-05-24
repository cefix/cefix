/*
 *  Timeline.cpp
 *  cefixSketch
 *
 *  Created by Stephan Huber on 26.10.09.
 *  Copyright 2009 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */


#include "Timeline.h"
#include <cefix/Serializer.h>
#include <cefix/Tweening.h>



namespace cefix {

Timeline::SamplerFactory::SamplerMap Timeline::SamplerFactory::_map;
std::string Timeline::SamplerFactory::_unknown("unknown");

Timeline::SamplerFunc Timeline::SamplerFactory::get(const std::string& name) 
{
	SamplerMap::iterator itr = _map.find(cefix::strToLower(name));
	if (itr == _map.end()) 
	{
		if (_map.size() == 0) {
			registerSampler("linear", &cefix::LinearTweening<double>::getValueAt );
			registerSampler(_unknown, &cefix::LinearTweening<double>::getValueAt );
			registerSampler("easyinout", &cefix::EasyInOutTweening<double>::getValueAt );
			
			registerSampler("QuadricIn",  &cefix::QuadricInTweening<double>::getValueAt );
			registerSampler("QuadricOut",  &cefix::QuadricOutTweening<double>::getValueAt );
			registerSampler("QuadricInOut",  &cefix::QuadricInOutTweening<double>::getValueAt );
			registerSampler("CubicIn",  &cefix::CubicInTweening<double>::getValueAt );
			registerSampler("CubicOut",  &cefix::CubicOutTweening<double>::getValueAt );
			registerSampler("CubicInOut",  &cefix::CubicInOutTweening<double>::getValueAt );
			registerSampler("QuartIn",  &cefix::QuartInTweening<double>::getValueAt );
			registerSampler("QuartOut",  &cefix::QuartOutTweening<double>::getValueAt );
			registerSampler("QuartInOut",  &cefix::QuartInOutTweening<double>::getValueAt );
			registerSampler("QuintIn",  &cefix::QuintInTweening<double>::getValueAt );
		 	registerSampler("QuintOut",  &cefix::QuintOutTweening<double>::getValueAt );
			registerSampler("QuintInOut",  &cefix::QuintInOutTweening<double>::getValueAt );
			registerSampler("SineIn",  &cefix::SineInTweening<double>::getValueAt );
			registerSampler("SineOut",  &cefix::SineOutTweening<double>::getValueAt );
			registerSampler("SineInOut",  &cefix::SineInOutTweening<double>::getValueAt );
			registerSampler("CircularIn",  &cefix::CircularInTweening<double>::getValueAt );
			registerSampler("CircularOut",  &cefix::CircularOutTweening<double>::getValueAt );
			registerSampler("CircularInOut",  &cefix::CircularInOutTweening<double>::getValueAt );
			registerSampler("ElasticIn",  &cefix::ElasticInTweening<double>::getValueAt );
			registerSampler("ElasticOut",  &cefix::ElasticOutTweening<double>::getValueAt );
			registerSampler("ElasticInOut",  &cefix::ElasticInOutTweening<double>::getValueAt );
			registerSampler("OvershootIn",  &cefix::OvershootInTweening<double>::getValueAt );
			registerSampler("OvershootOut",  &cefix::OvershootOutTweening<double>::getValueAt );
			registerSampler("OvershootInOut",  &cefix::OvershootInOutTweening<double>::getValueAt );
			registerSampler("BounceIn",  &cefix::BounceInTweening<double>::getValueAt );
			registerSampler("BounceOut",  &cefix::BounceOutTweening<double>::getValueAt );
			registerSampler("BounceInOut",  &cefix::BounceInOutTweening<double>::getValueAt );
		}
		
		if (name != "linear") 
			log::error("Timeline") << "could not find sampler " << name << ", returning linear" << std::endl;
		return get("linear");
	}
	
	return itr->second;
}


void Timeline::SamplerFactory::registerSampler(const std::string& name, SamplerFunc sampler) 
{
	_map[cefix::strToLower(name)] = sampler;
}

const std::string& Timeline::SamplerFactory::getName(SamplerFunc func)
{
	for(SamplerMap::iterator i = _map.begin(); i != _map.end(); ++i) {
		if (i->second == func)
			return i->first;
	}
	return _unknown;
}


Timeline::Action::Action(double starttime, double duration) 
:	osg::Referenced(), 
	_startTime(starttime), 
	_duration(duration), 
	_active(false), 
	_samplerFunc(SamplerFactory::get("linear")),
	_callWithinWhenLeaving(true),
	_loop(false),
	_name("unnamed action") 
{
}

Timeline::Action::Action() 
:	osg::Referenced(), 
	_startTime(0), 
	_duration(0), 
	_active(false), 
	_samplerFunc(SamplerFactory::get("linear")), 
	_callWithinWhenLeaving(true),
	_loop(false),
	_name("unnamed action")
{
}

void Timeline::Action::readFrom(cefix::Serializer& serializer)
{
	std::string sampler;
	serializer >> "name"		>> serializer.defaultValue(_name, "unnamed action");
	serializer >> "starttime"	>> _startTime;
	serializer >> "duration"	>> _duration;
	serializer >> "sampler"		>> sampler;
	serializer >> "loop"		>> serializer.defaultValue(_loop, false);
	
	if (!sampler.empty())
		setSampler(sampler);	
}

void Timeline::Action::writeTo(cefix::Serializer& serializer)
{
	serializer << "name"		<< _name;
	serializer << "starttime"	<< _startTime;
	serializer << "duration"	<< _duration;
	
    std::string sampler = SamplerFactory::getName(_samplerFunc);
    
    if(sampler != "linear")
        serializer << "sampler"		<< sampler;
	
    if (_loop)
        serializer << "loop"		<< _loop;
}


void TimelineAsActionAdapter::readFrom(cefix::Serializer& serializer)
{
	serializer >> "name"		>> serializer.defaultValue(_name, "unnamed action");
	serializer >> "starttimeDelta" >> _startTimeDelta;
	serializer >> "timeline" >> _tl;
	recalculateStartTimeAndDuration();
	
}

void TimelineAsActionAdapter::writeTo(cefix::Serializer& serializer)
{
	serializer << "name"			<< _name;
	serializer << "starttimeDelta"	<< _startTimeDelta;
	serializer << "timeline"		<< _tl;
}


Timeline::Timeline() 
:	osg::Referenced(), 
	_currentTime(0.0), 
	_startTime(std::numeric_limits<double>::max()), 
	_endTime(0.0), 
	_duration(0.0),
	_callMissedActions(true),
	_callMissedActionsThreshold(0.05),
	_name("unnamed timeline"),
	_inited(false) 
{
}

    
void Timeline::clear() {
    _data.clear();
    _activeActions.clear();

    _currentTime = 0.0;
	_startTime = std::numeric_limits<double>::max();
	_endTime = 0.0;
	_duration = 0.0;
    _inited = false;
}

std::ostream& Timeline::dump(std::ostream& os)
{
	os << this << std::endl;
	for(Data::iterator i = _data.begin(); i != _data.end(); ++i) {
		os << "  " << i->second.get() << std::endl;
	}
	
	return os;
}

void Timeline::callMissedActions(double old_time, double new_time) 
{
	double t_min = std::min(old_time, new_time);
	double t_max = std::max(old_time, new_time);
	double scalar = old_time > new_time ? 0.0 : 1.0;
	
	//std::cout << "missed actions for timeline: " << this << " time-frame: " << t_min << " " << t_max << std::endl;
	//dump(std::cout);
	
	// wir gehen die liste vom höchsten Element zum nidrigsten durch:
	
	Data::iterator itr = _data.upper_bound(t_max);
	if (itr != _data.begin())
		--itr;
	
	//std::cout << "  upper:  " << itr->second.get() << std::endl;
	
	std::list<Action*> missed_actions;
	
	while (itr != _data.end()  && itr->second->intersects(t_min, t_max) )
	{
		Action* a = itr->second.get();
		if (!a->isActive() && (!a->contains(new_time))) 
		{
			missed_actions.push_back(a);
			// std::cout << "  missed: " << (a) << std::endl;
		} else {
			; //std::cout << "  later:  " << (a) << std::endl;
		}
		if (itr == _data.begin())
			break;
		else
			--itr;
	}
	for(std::list<Action*>::iterator i = missed_actions.begin(); i != missed_actions.end(); ++i) {
		
		(*i)->enter();
		(*i)->handleWithin(scalar);
		(*i)->handleLeave(new_time);
	}

}

void Timeline::readFrom(cefix::Serializer& serializer)
{
	serializer.registerClass<TimelineAsActionAdapter>("TimelineAsActionAdapter");
	serializer >> "name" >> serializer.defaultValue(_name, "unnamed timeline");
	serializer >> "callMissedActions" >> serializer.defaultValue(_callMissedActions, true);
	serializer >> "callMissedActionsThreshold" >> serializer.defaultValue(_callMissedActionsThreshold, 0.05);
	serializer >> "data" >> _data;
	recalculateStartTimeAndDuration();
	
	for(iterator i = begin(); i != end(); ++i) {
		i->second->setParentTimeline(this);
	}
}

void Timeline::writeTo(cefix::Serializer& serializer)
{
	serializer.registerClass<TimelineAsActionAdapter>("TimelineAsActionAdapter");
	
	serializer << "name" << _name;
	serializer << "starttime" << _startTime;
	serializer << "duration" << _duration;
	serializer << "callMissedActions" << _callMissedActions;
	serializer << "callMissedActionsThreshold" << _callMissedActionsThreshold;
	serializer << "data" << _data;
	
}

void Timeline::add(Timeline* tl, double starttime)
{
	tl->recalculateStartTimeAndDuration();
	add(new TimelineAsActionAdapter(tl, starttime));
	
	recalculateStartTimeAndDuration();
}



void TimelineAsActionAdapter::init() 
{
	cefix::Timeline::Action::init();
	_tl->setCallMissedActionsFlag(getParentTimeline()->getCallMissedActionsFlag(), getParentTimeline()->getCallMissedActionsThreshold());
	_tl->init();
	bool has_looping_action = false;
	for(Timeline::iterator i = _tl->begin(); i != _tl->end(); ++i) {
		if (i->second->isLooping()) {
			has_looping_action = true;
			break;
		}
	}
	if (has_looping_action) 
	{
		
		setDuration(getParentTimeline()->getDuration());
	}
	setName("TimelineAdapter for " + _tl->getName());
}

}