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

#ifndef CEFIX_SOUND_STATE_OBSERVER_TREAD
#define CEFIX_SOUND_STATE_OBSERVER_TREAD

#include <OpenThreads/Thread>
#include <OpenThreads/Mutex>
#include <osg/Referenced>
#include "Sound.h"


namespace cefix {

class SoundStateObserverThread : public OpenThreads::Thread, public osg::Referenced {

public:

	typedef std::list< osg::observer_ptr<Sound> > SoundList;

	SoundStateObserverThread() 
	:	OpenThreads::Thread(), 
		osg::Referenced(),
		_isRunning(false),
		_stopped(false),
		_numPlayingSounds(0)
	{
		start();
	}
	
	static SoundStateObserverThread* getInstance();
	
	virtual void run();
	
	~SoundStateObserverThread() 
	{
		_stopped = true;
		_block.release();
		
		while(_isRunning) {
			OpenThreads::Thread::microSleep(10*1000);
		}
	}
	
	void addSound(Sound* s) {
		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
		// std::cout << "release observer thread" << std::endl;
		_sounds.push_back(s);
		_block.release();
	}
	
	inline unsigned int getNumPlayingSounds() { return _numPlayingSounds; }

private:
	bool				_isRunning, _stopped;
	OpenThreads::Mutex	_mutex;
	SoundList			_sounds;
	OpenThreads::Block	_block;
	unsigned int		_numPlayingSounds;
	
	
};


}

#endif