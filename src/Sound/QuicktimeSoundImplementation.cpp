/*
 *  QuicktimeSoundImplementation.cpp
 *  render_tests
 *
 *  Created by Stephan Huber on 15.10.09.
 *  Copyright 2009 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#if defined CEFIX_QUICKTIME_AVAILABLE


#include "QuicktimeSoundImplementation.h"
#include "SoundManager.h"


namespace cefix {


void QuicktimeSoundImplementation::open(const Sound::Source& source)
{
	_movie = new cefix::QuicktimeMovie(source.getFileName());
	
	if (source.getStreamed() == false) {
		_movie->preload();
	}
}


bool QuicktimeSoundImplementation::play()
{
	_movie->play();
	return true;
}


bool QuicktimeSoundImplementation::pause()
{
	_movie->pause();
	return true;
	
}


bool QuicktimeSoundImplementation::stop()
{
	_movie->pause();
	return true;
}


void QuicktimeSoundImplementation::setVolume(float volume)
{
	_movie->setVolume(volume);
	_movie->idle();
}


void QuicktimeSoundImplementation::setBalance(float balance)
{
	_movie->setAudioBalance(balance);
}


void QuicktimeSoundImplementation::setPitch(float pitch) 
{
    _movie->setTimeMultiplier(pitch);
}


double QuicktimeSoundImplementation::getDuration()
{
	return _movie->getLength();
}


double QuicktimeSoundImplementation::getCurrentTime()
{
	return _movie->getCurrentTime();
}


void QuicktimeSoundImplementation::setLooping(bool doLoop)
{
	_movie->setLoopingMode(doLoop ? cefix::QuicktimeMovie::LOOPING : cefix::QuicktimeMovie::NO_LOOPING);
}


void QuicktimeSoundImplementation::seek(double t)
{
	_movie->jumpTo(t);
	_movie->idle();
}




}

#endif