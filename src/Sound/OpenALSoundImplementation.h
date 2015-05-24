/*
 *  OpenALSoundImplementation.h
 *  dm_musikautomaten
 *
 *  Created by Stephan Huber on 09.12.09.
 *  Copyright 2009 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#ifndef OPEN_AL_SOUND_IMPLEMANTATION_HEADER
#define OPEN_AL_SOUND_IMPLEMANTATION_HEADER


#include "Sound.h"

#ifdef __OBJC__
@class OALAudioTrack;
@class ALSource;
@class ALBuffer;
#else
class OALAudioTrack;
class ALSource;
class ALBuffer;
#endif

namespace cefix {

class OpenALSoundImplementation : public Sound::Implementation {
	public:
		OpenALSoundImplementation();
		
		virtual void open(const Sound::Source& source);
		virtual bool play();
		virtual bool pause();
		virtual bool stop();
		virtual void setVolume(float volume);
		virtual void setBalance(float balance);
        virtual void setPitch(float pitch);
		virtual double getDuration();
		virtual double getCurrentTime();
		virtual void setLooping(bool doLoop);
		virtual void seek(double t);
		virtual bool valid() const { return _valid; }
		
		virtual bool isPlaying();
		
		OALAudioTrack* getTrack() { return _track; }
		
	protected:
		~OpenALSoundImplementation();
		bool _valid;
		OALAudioTrack* _track;
		ALSource* _source;
		ALBuffer* _buffer;
		
	};
	
}


#endif