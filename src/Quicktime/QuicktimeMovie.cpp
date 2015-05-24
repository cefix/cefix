/*
 *  QuicktimeMovie.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 02.05.06.
 *  Copyright 2006 digitalmind.de // Stephan Maximilian Huber. All rights reserved.
 *
 */

#if defined CEFIX_QUICKTIME_AVAILABLE

#include <cefix/QuicktimeMovie.h>
#include <cefix/MovieData.h>
#include <cefix/Settings.h>

#ifdef CEFIX_CORE_VIDEO_AVAILABLE
    #include <cefix/CoreVideoAdapter.h>
#endif

namespace cefix {

	bool QuicktimeMovie::s_quicktime_playing_disabled = false;

      
            
// ----------------------------------------------------------------------------------------------------------
// ctro
// ----------------------------------------------------------------------------------------------------------

QuicktimeMovie::QuicktimeMovie(std::string file) : 
	cefix::Video(),
	VideoMediaAdapter<QuicktimeMovie>("quicktime"),
	_currentRate(0),
	_data(NULL)
 {
	_data = new MovieData();
		
	if (!file.empty())
		load(file);   
}

// ----------------------------------------------------------------------------------------------------------
// dtor
// ----------------------------------------------------------------------------------------------------------

QuicktimeMovie::~QuicktimeMovie() {
	if (s_quicktime_playing_disabled)
		osg::notify(osg::ALWAYS) << "destructing" << getFileName() << std::endl;
    
    clear();
	
    if (_data)
		delete _data;
	_data = NULL;
}

    /// Start or continue stream.
    void QuicktimeMovie::play() {
        setMovieRate(1.0f);
    }
    
    /// sets the movierate
    void QuicktimeMovie::setMovieRate(double rate) {
		if (s_quicktime_playing_disabled) return;

		_data->setLooping(getLoopingMode() == LOOPING);
		_currentRate = rate;
        _data->setMovieRate(rate);
        setRunning((rate!=0));
    }
	
	double QuicktimeMovie::getMovieRate() const {
		return _data->getMovieRate();
	}
    
    /// Pause stream at current position.
    void QuicktimeMovie::pause() {
        setMovieRate(0.0f);
		dirty();
    }
    
    /// Rewind stream to beginning.
    void QuicktimeMovie::rewind() {
        _data->setMovieTime(0.0f);
		setCurrentTime(_data->getMovieTime());
		dirty();
    }
    
    /// forward stream to the end
    void QuicktimeMovie::forward() {
        _data->setMovieTime(getLength());
		setCurrentTime(_data->getMovieTime());
		dirty();
    }
    
    
    /// stop playing 
    void QuicktimeMovie::quit(bool b) {
        pause();
    }
	
	void QuicktimeMovie::setVolume(float v) {
		_data->setVolume(v);
	}
	
	float QuicktimeMovie::getVolume() const {
		return _data->getVolume();
	}
	
	float QuicktimeMovie::getAudioBalance() {
		return _data->getAudioBalance();
	}
	
	void QuicktimeMovie::setAudioBalance(float b) {
		_data->setAudioBalance(b);
	}
	
	
	void QuicktimeMovie::flushUsedRam() {
		if (!getCoreVideoAdapter())
			_data->reopen();
	}
    
    
    void QuicktimeMovie::jumpTo(double time) {
        _data->setMovieTime(time);
        setCurrentTime(_data->getMovieTime());dirty();
    }
    
    void QuicktimeMovie::checkLoop() {
        float cachedMovieRate = _data->getCachedMovieRate();
        if ((_currentTime <= 0.0f) && (cachedMovieRate < 0.0f)) {
            forward();
            setMovieRate(cachedMovieRate);
        } 
        // loppen wir vorwärts?
        else if ((_currentTime >= _len) && (cachedMovieRate > 0.0f)) {
            rewind();
            setMovieRate(cachedMovieRate);
        }
    }
	
	void QuicktimeMovie::checkIfMovieIsDone() {
		if (IsMovieDone(_data->getMovie()))
			setMovieRate(0);
	}

        
    /// loads a movie from fileName
    void QuicktimeMovie::load(const std::string& fileName) 
        {
        
        osg::notify(osg::DEBUG_INFO) << "QT-ImageStream: loading quicktime movie from " << fileName << std::endl;

        _data->load(this, fileName);
        _len = _data->getMovieDuration();
        setCurrentTime(0);
   
    }
	
	void QuicktimeMovie::preload(float startTime, float duration)
	{
		_data->preload(startTime, duration);
	}
	
	float QuicktimeMovie::getVideoWidth() {
		return _data->getMovieWidth();
	}
	
	float QuicktimeMovie::getVideoHeight() {
		return _data->getMovieHeight();
	}

    
	void QuicktimeMovie::deactivate() {
		_data->deactivate();
	}
	
	void QuicktimeMovie::activate() {
		_data->activate();
	}
	
	bool QuicktimeMovie::isActive() const {
		return _data->isActive();
	}
	
	bool QuicktimeMovie::valid() const
	{
		return (_data && _data->valid());
	}
	
	

	void QuicktimeMovie::idle() {
		float lastcurrent = getCurrentTime();
		MoviesTask(getMovieData()->getMovie(),0);
		setCurrentTime(getMovieData()->getMovieTime());
		checkForUpdate();
		
		if (lastcurrent != getCurrentTime() ) 
			traverseCallbacks(this);
	}
    
    
    void QuicktimeMovie::checkForUpdate() 
    {
        if (_currentTime != _lastUpdate) {
            if (getMovieData()->hasVideoTrack()) {
                setData(getMovieData()->swapBuffers(), getAllocationMode());
                dirty();
            }
            _lastUpdate = _currentTime;
        }
        else {
            if ((_currentRate != 0) && ((_currentTime <=0.0) || (_currentTime >= _len)))
                checkIfMovieIsDone();
        }

    }
    
    
    void QuicktimeMovie::setCoreVideoAdapter(CoreVideoAdapter* adapter)
    {
        #ifdef CEFIX_CORE_VIDEO_AVAILABLE
            _coreVideoAdapter = adapter;
            SetMovieVisualContext(getMovieData()->getMovie(), _coreVideoAdapter ? _coreVideoAdapter->getVisualContext() : NULL);
            MoviesTask(getMovieData()->getMovie(),0);
        #endif
    }
    
}

#endif