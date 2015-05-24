/*
 *  QuicktimeSoundImplementation.h
 *  render_tests
 *
 *  Created by Stephan Huber on 15.10.09.
 *  Copyright 2009 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#ifndef QUICKTIME_SOUND_IMPLEMENTATION_HEADER
#define QUICKTIME_SOUND_IMPLEMENTATION_HEADER

#include <cefix/Export.h>

#if defined CEFIX_QUICKTIME_AVAILABLE

#include <cefix/Sound.h>
#include <cefix/QuicktimeMovie.h>



namespace cefix {

class QuicktimeSoundImplementation : public Sound::Implementation {
	public:
		QuicktimeSoundImplementation() : Sound::Implementation() {}
		
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
		virtual bool valid() const { return _movie.valid(); }
		
		virtual bool isPlaying() { return _movie->isPlaying(); }
		
	protected:
		osg::ref_ptr<cefix::QuicktimeMovie> _movie;
	};
	
}
#endif

#endif