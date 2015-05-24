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

#ifndef CEFIX_TIMELINE_HEADER
#define CEFIX_TIMELINE_HEADER

#include <map>
#include <list>
#include <limits>
#include <osg/ref_ptr>
#include <osg/Referenced>
#include <cefix/AnimationBase.h>
#include <cefix/StringUtils.h>


namespace cefix {


class Serializer;



class Timeline : public osg::Referenced {

public: 
	typedef double(*SamplerFunc)(double);
	
	class SamplerFactory {
	
	public:
		typedef std::map<std::string, SamplerFunc> SamplerMap;
		
		static SamplerFunc get(const std::string& name);
		static void registerSampler(const std::string& name, SamplerFunc func);
		static const std::string& getName(SamplerFunc func);
	
	private:
		static SamplerMap _map;
		static std::string _unknown;
	};
	
	/** 
		base class for any timeline action, you should implement enter, within and leave. Every action has a start-time and a duration. 
	    If the playhead enters your time-frame, enter() will be called. As long the playhead is inside your action within(scalar) will be called. 
		if the playhead has left your action leave() gets called.
	*/
	class Action : public osg::Referenced {

	public:
		/// ctor
		Action(double starttime, double duration);
		
		/// get the start time
		inline double getStartTime() const { return _startTime; }
		
		/// get the end time
		inline double getEndTime() const { return _startTime + _duration; }
		
		/// get the duration
		inline double getDuration() const { return _duration; }
		
		virtual void readFrom(cefix::Serializer& serializer);
		virtual void writeTo(cefix::Serializer& serializer);
		
		/// sets the SamplerFunc
		void setSamplerFunc(SamplerFunc f) { _samplerFunc = f; }
		
		/// sets the sampler by a given name (register your samplers via the SamplerFactory )
		void setSampler(const std::string& name) { setSamplerFunc(SamplerFactory::get(name)); }
		
		/// sets starttime and duration
		void setStartTimeAndDuration(double s, double d) { setStartTime(s); setDuration(d); }
		
		inline void setDuration(double d) { _duration = d; }
		inline void setStartTime(double s) { _startTime = s; }
		
		/// returns true, if this actions intersects with the given temporal range
		bool intersects(double t_start, double t_end) const { 
			return (((t_start <= _startTime) && (_startTime <= t_end)) || ((t_start <= _startTime+_duration) && (_startTime+_duration <= t_end))); 
		}
		
		/// get the name
		inline const std::string& getName() const { return _name; }
		
		/// set the name
		inline void setName(const std::string& name) { _name = name; }
		
		void setLoop(bool f ) { _loop = f; }
		
		bool isLooping() const { return _loop; }
		
	protected:
	
		/// ctor
		Action();
		
		/// sets the parent timeline
		void setParentTimeline(Timeline* tl) { _timeline = tl; }
		
		/// returns true, if this action "contains" time t
		inline bool contains(double t) { return ((_startTime <= t) && ( _loop || (t <= (_startTime + _duration)))); }
		
		/** abstract function enter, called, when the playhead is inside  this action the first time */
		virtual void enter() {}; 
		
		/** abstract function, gets called, when the playhead is inside this action */
		virtual void within(double scalar) {};
		
		/** abstract function, gets called, when the playhed has left this action */
		virtual void leave() {}; 
		
		/** abstract function, gets called, when the timeline gets inited */
		virtual void init() {}

		/** returns true, if this action is currently active (the playhead is inside this action) */
		inline bool isActive() const { return _active; }
		
		/** set the active flag for this action (only called by the timeline, don't call it by yourself) */
		inline void setActiveFlag(bool b) { _active = b; }
		
		/// recalculate the start time + duration
		virtual void recalculateStartTimeAndDuration() {}
		
		/// get the SamplerFunc used by this action
		inline SamplerFunc getSampler() { return _samplerFunc; }
		
		// if set to true, this action's within-method gets called with a proper scalar (0.0 / 1.0), when the playhead leaves the action. 
		inline void setCallWithinWhenLeaving(bool f) { _callWithinWhenLeaving = f; }
		
		/// getter for the call WhenLeaving-flag
		inline bool getCallWithinWhenLeaving() const { return _callWithinWhenLeaving; }
		
		/// handle the callWithinWhenLeaving-flag when leaving, called by the timeline
		inline void handleLeave(double t) 
		{
			if (getCallWithinWhenLeaving()) {
				if (t > getEndTime())
					handleWithin(1.0);
				else
					handleWithin(0.0);
			}
			
			leave();
		}
		
		inline void handleWithin(float scalar) 
		{
			within( (*_samplerFunc)(_loop ? wrapAround(scalar) : scalar) );
		}
	
		inline double wrapAround(double a) const
		{
			return ((a*2) - static_cast<double>( static_cast<int>( (a*2) / 2 ) ) * 2) / 2.0;
		}
	
	protected:
		inline Timeline* getParentTimeline() { return _timeline; }

	private:
		double _startTime, _duration;
		bool   _active;
		Timeline*	_timeline;
		SamplerFunc _samplerFunc;
		bool		_callWithinWhenLeaving, _loop;
	protected:
		std::string _name;
		
	friend class Timeline;
	friend class Serializer;
	
	};

	typedef std::multimap<double, osg::ref_ptr<Action> > Data;
	typedef Data::iterator iterator;
	typedef Data::const_iterator const_iterator;
	typedef std::list<Action*> ActiveActionList;
	
	/// ctor
	Timeline();
	
	/// returns a begin-iterator
	iterator begin() { return _data.begin(); }
	
	/// returns an end-iterator
	iterator end() { return _data.end(); }

	/// returns a const begin-iterator
	const_iterator begin() const { return _data.begin(); }
	
	/// returns a const end-iterator
	const_iterator end() const { return _data.end(); }
    
    /// clears all actions from the timeline
    void clear();

	
	/// add an action, ajust duration and starttime of this timeline
	void add(Action* action) 
	{ 
		if (!action) return;
		
		action->setParentTimeline(this);
		
		_data.insert(std::make_pair(action->getStartTime(), action)); 
		
		// compute starttime + duration
		_startTime = std::min(_startTime, action->getStartTime());
		_endTime  = std::max(_endTime, action->getEndTime() );
		_duration = _endTime - _startTime;
		
		newActionAdded(action);
	}
	

	
	/// suitable for inherited classes, get called, after an action is added to the timeline
	virtual void newActionAdded(Action* action) {}
	
	/// add a timeline to this timeline
	void add(Timeline* tl, double starttime = 0.0);
	
	/// sets the time of the playhead 
	void setTime(double t) 
	{
		if (!_inited) init();
		//std::cout << getName() << " setTime: " << t << std::endl;
		
		double last_current_time = _currentTime;
		_currentTime = t;
		
		if ((_callMissedActions) && (std::abs(last_current_time - _currentTime) > _callMissedActionsThreshold)) 
		{
			callMissedActions(last_current_time, _currentTime);
		}
		
		//std::cout << "active actions before: " << _activeActions.size();
		// bestehende Actions bedienen
		for(ActiveActionList::iterator i = _activeActions.begin(); i != _activeActions.end();) 
		{
			
			if ((*i)->contains(t)) 
			{
				//std::cout << "saved action " << (*i) << " " << (*i)->getStartTime() << "-" << (*i)->getEndTime() << std::endl;
				
				(*i)->handleWithin( (t-(*i)->getStartTime()) / (*i)->getDuration() );
				++i;
			}
			else {
				(*i)->handleLeave(t);
				(*i)->setActiveFlag(false);
				i = _activeActions.erase(i);
			}
		}
		
		//std::cout << " middle: " << _activeActions.size();
		// neue actions für die Zeit finden
		
		// upper_bound liefert eintrag wo der Key > als der such-key ist
		Data::iterator itr = _data.upper_bound(t);
	
		if (itr != _data.begin()) {
			--itr;
		}
		
		//std::cout << "upper: " << itr->second.get() << " " << itr->second->getStartTime() << "-" << itr->second->getEndTime() << std::endl;
		
		while (itr != _data.end() && (itr->first <= t)) 
		{
			if (!itr->second->isActive() && (itr->second->contains(t))) 
			{
				// std::cout << "new action   " << itr->second.get() << std::endl;
				Action* action = itr->second.get();
				_activeActions.push_back(action);
				action->setActiveFlag(true);
				action->enter();
				action->handleWithin( (t-action->getStartTime()) / action->getDuration() );
			}
			if (itr == _data.begin())
				break;
			else
				--itr;
		}	
	}
	
	/// get the current time
	inline double getCurrentTime() const { return _currentTime; }
	
	/// clear all pending active actions
	void clearActiveActions() {
		for(ActiveActionList::iterator i = _activeActions.begin(); i != _activeActions.end(); ) 
		{
			(*i)->handleLeave(_currentTime);
			(*i)->setActiveFlag(false);
			i = _activeActions.erase(i);
		}
	}
	
	/// get the duration of this timeline
	inline double getDuration() const { return _duration; }
	
	/// get the starttime of this timeline
	inline double getStartTime() const { return _startTime; }
	
	inline double getEndTime() const { return _endTime; }
	
	// recalculate starttime + duration for this timeline
	void recalculateStartTimeAndDuration() 
	{
		_duration = 0.0;
		_endTime = 0.0;
		_startTime = std::numeric_limits<double>::max();
		
		for(Data::iterator i = _data.begin(); i != _data.end(); ++i) {
			i->second->recalculateStartTimeAndDuration();
			_endTime = std::max(_endTime, i->second->getEndTime());
			_startTime = std::min(_startTime, i->second->getStartTime());
		}
		_duration = _endTime - _startTime;
	}

	virtual void readFrom(cefix::Serializer& serializer);
	virtual void writeTo(cefix::Serializer& serializer);
	
	// init this timeline and all actions
	virtual void init() 
	{
		for(Data::iterator i = _data.begin(); i != _data.end(); ++i) {
			i->second->init();
		}
		_inited = true;
	}
	
	/** 
	 * if this flag is set to true, all actions between the last time and the current time get handled, even if the temporal jump 
	 * was big enough to not touch an action. This defaults to true. do the check only if the jump in time is greater than the given threshold
	 * if set to true, the wohle timeline gets resetted when you "rewind" to the beginning.
	 */
	inline void setCallMissedActionsFlag(bool f, double threshold) { _callMissedActions = f; _callMissedActionsThreshold = threshold; }
	
	/// get the callMissedActions flag
	inline bool getCallMissedActionsFlag() const { return _callMissedActions; }
	
	/// get the threshold
	inline double getCallMissedActionsThreshold() const { return _callMissedActionsThreshold; }
	
	/// get the name
	inline const std::string& getName() const { return _name; }
	
	/// set the name
	void setName(const std::string& name) { _name = name; }
	
	// get the num of added actions
	std::size_t getNumActions() { return _data.size(); }
	
	/// dump all actions to the stream (for debug purposes)
	std::ostream& dump(std::ostream& os);
	
protected:
	void callMissedActions(double old_time, double new_time);
	
private:
	Data					_data;
	ActiveActionList		_activeActions;
	double					_currentTime, _startTime, _endTime, _duration;
	bool					_callMissedActions;
	double					_callMissedActionsThreshold;
	std::string				_name;
	bool					_inited;

};


class TimelineAsActionAdapter : public Timeline::Action {
public:
	TimelineAsActionAdapter() : Timeline::Action(0,0), _tl(NULL), _startTimeDelta(0) {}

	TimelineAsActionAdapter(Timeline* tl, double delta = 0) : Timeline::Action(tl->getStartTime() + delta, tl->getDuration()), _tl(tl), _startTimeDelta(delta) {}
	
	virtual void enter() {}
	virtual void within(double scalar) 
	{
		_tl->setTime( getParentTimeline()->getCurrentTime() - _startTimeDelta );
	}
	
	virtual void leave() {
		_tl->clearActiveActions();
	}
	
	virtual void recalculateStartTimeAndDuration() {
		_tl->recalculateStartTimeAndDuration();
		setStartTimeAndDuration(_tl->getStartTime() + _startTimeDelta, _tl->getDuration()); 
	}
	
	virtual void init();
	
	void readFrom(cefix::Serializer& serializer);
	void writeTo(cefix::Serializer& serializer);
	
	Timeline* getTimeline() { return _tl.get(); }

private:
	osg::ref_ptr<Timeline> _tl;
	double					_startTimeDelta;
	

};





/// small helper class so you can add a timeline as an animation to an animationcontroller /-factory
class TimelineAnimation : public cefix::AnimationBase {

public:
	TimelineAnimation(Timeline* tl, bool doLoop = false) : cefix::AnimationBase(tl->getStartTime()), _tl(tl), _doLoop(doLoop) {}
	
protected:
	virtual void cleanup() {
		_tl->clearActiveActions();
	}
	
	virtual void animate(float elapsed) {
		while(_doLoop && (elapsed > _tl->getEndTime()))
			elapsed -= _tl->getEndTime();
			
		_tl->setTime(elapsed);
		if (!_doLoop && (elapsed >= _tl->getEndTime())) {
			setFinishedFlag();
		}
	}
private:
	osg::ref_ptr<Timeline>	_tl;
	bool					_doLoop;

};


inline std::ostream& operator<<(std::ostream& os, cefix::Timeline* tl) {
	os << tl->getName()   << " (" << (&tl) << ") " << tl->getStartTime() << "-" << tl->getEndTime() << " # actions: " << tl->getNumActions();
	return os;
}

inline std::ostream& operator<<(std::ostream& os, cefix::Timeline::Action* a) {
	os << a->getName() << " (" << (&a) << ") " << a->getStartTime() << "-" << a->getEndTime();
	return os;
}

}

#endif