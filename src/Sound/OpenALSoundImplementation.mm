/*
 *  OpenALSoundImplementation.cpp
 *  dm_musikautomaten
 *
 *  Created by Stephan Huber on 09.12.09.
 *  Copyright 2009 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "OpenALSoundImplementation.h"
#include <cefix/DataFactory.h>

#import "ObjectAL.h"

namespace cefix {


OpenALSoundImplementation::OpenALSoundImplementation() 
:	Sound::Implementation(), 
	_valid(false),
	_track(NULL),
	_source(NULL),
	_buffer(NULL)
{

}

void OpenALSoundImplementation::open(const Sound::Source& source)
{
	if (source.getStreamed()) 
	{
		_track = [OALAudioTrack track];
		[_track retain];
		
		[_track preloadFile: [NSString stringWithUTF8String: source.getFileName().c_str()]];
		_track.autoPreload = YES;
		_track.numberOfLoops = _sound->isLooping() ? -1 : 0;
		
		_valid = true;

	} else {
		// use open al
		[OALSimpleAudio sharedInstance].reservedSources = 0;
	
		_source = [[ALSource source] retain];
		_buffer = [[[OpenALManager sharedInstance] bufferFromFile: [NSString stringWithUTF8String: source.getFileName().c_str()] reduceToMono:YES] retain];
		
		_valid = true;		
	}
}



OpenALSoundImplementation::~OpenALSoundImplementation() 
{
	if(_track) {
		[_track close];
		[_track release];
		_track = NULL;
	}
	if(_source) {
		[_buffer release];
		[_source release];
	}
}


bool OpenALSoundImplementation::play()
{
	if (_track) {
		if ([_track paused])
			[_track setPaused: 0];
		else
			[_track play];
	}
	else {
		if ([_source paused])
			[_source setPaused: 0];
		else
			[_source play:_buffer loop: _sound->isLooping()];
	}
	return true;
}

bool OpenALSoundImplementation::pause()
{
	if (_track) 
	{
		[_track setPaused: true];
	} 
	else 
	{
		[_source setPaused: true];
	}
	return true;
}


bool OpenALSoundImplementation::stop()
{
	if (_track) 
	{
		[_track stop];
	} 
	else 
	{
		pause();
	}
	return true;
}



void OpenALSoundImplementation::setVolume(float volume)
{
	if (_track) 
	{
		[_track setVolume: volume];
	} 
	else 
	{
		[_source setVolume: volume];
	}
}



void OpenALSoundImplementation::setBalance(float balance)
{
	if (_track) 
	{
		[_track setPan: balance];
	} 
	else 
	{
		[_source setPan: balance];
	}
}

void OpenALSoundImplementation::setPitch(float pitch)
{
	if (_track) 
	{
		cefix::log::error("OpenALSoundImplementation") << "can't pitch a sound track" << std::endl;
	} 
	else 
	{
		[_source setPitch: pitch];
	}
}


double OpenALSoundImplementation::getDuration()
{
	if (_track) 
	{
		return [_track duration];
	} 
	else 
	{
		return [_buffer duration];
	}
	return 0.0; // satisfy compiler
}



double OpenALSoundImplementation::getCurrentTime()
{
	if (_track) 
	{
		return [_track currentTime];
	} 
	else 
	{
		return [_source offsetInSeconds];
	}
	return 0.0; // satisfy compiler
}



void OpenALSoundImplementation::setLooping(bool doLoop)
{
	if (_track) 
	{
		[_track setNumberOfLoops: (doLoop) ? -1 : 0];
	} 
	else 
	{
	
	}
}



void OpenALSoundImplementation::seek(double t)
{
	if (_track) 
	{
		[_track setCurrentTime: t];
	} 
	else 
	{
        _source.offsetInSeconds = t;
	}
}

bool OpenALSoundImplementation::isPlaying() 
{ 
	if (_track) 
	{
		return ([_track playing] && ![_track paused]);
	} 
	else 
	{
		return ([_source playing] && ![_source paused]);
	} 
	
	return false;
}


}


