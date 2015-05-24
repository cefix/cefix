/*
 *  SoundStateObserverThread.cpp
 *  dm_musikautomaten
 *
 *  Created by Stephan Huber on 21.12.09.
 *  Copyright 2009 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "SoundStateObserverThread.h"
#include <iostream>

namespace cefix {

SoundStateObserverThread* SoundStateObserverThread::getInstance() 
{
	static osg::ref_ptr<SoundStateObserverThread> s_ptr = new SoundStateObserverThread();
	return s_ptr.get();
}

void SoundStateObserverThread::run() 
{
	_isRunning = true;
	while (!_stopped) 
	{
		bool should_block(false);
		{
			OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
			// std::cout << "check playing state" << std::endl;
			if (0) {	
				static osg::Timer_t last_status = osg::Timer::instance()->tick();
				if (osg::Timer::instance()->delta_s(last_status, osg::Timer::instance()->tick()) > 0.5) {
					std::cout << "# sounds: " << _sounds.size() << std::endl;
					last_status = osg::Timer::instance()->tick();
				}
			}
			_numPlayingSounds = _sounds.size();
			
			for(SoundList::iterator i = _sounds.begin(); i != _sounds.end(); )
			{
				bool is_playing = false;
				if (i->valid()) {
					(*i)->checkPlayingState();
					is_playing = (*i)->isPlaying();
				}
				
				if (is_playing) {
					++i;
				} else {
					i = _sounds.erase(i);
				}
			}
			
			should_block = (_sounds.size() == 0);
		}
		if (should_block) {
			// std::cout << "block observer thread" << std::endl;
			_block.reset();
			_block.block();
		} else {
			OpenThreads::Thread::microSleep(10*1000);
		}		
	}
	_isRunning = false;
}


}