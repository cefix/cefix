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

#ifndef CEFIX_SOUND_HEADER
#define CEFIX_SOUND_HEADER

#include <string>
#include <osg/Referenced>
#include <osg/ref_ptr>
#include <osgDB/FileUtils>

namespace cefix {

class Sound : public osg::Referenced {
public:
	
	class Source {
	public:
		Source(const std::string& filename, bool streamed = false) 
		:	_filename(osgDB::findDataFile(filename)), 
			_data(0),
			_streamed(streamed) 
		{
			setVolume(1.0f);
			setBalance(0.0f);
			setPitch(1.0f);
            setLooping(false);
			setAutoPlay(false);
            
		}
		
		Source(void* data, bool streamed) 
		:	_filename(""), 
			_data(data),
			_streamed(streamed) 
		{
			setVolume(1.0f);
			setBalance(0.0f);
			setPitch(1.0f);
            setLooping(false);
			setAutoPlay(false);
		}
		
		
		bool isFile() const { return (!_filename.empty() && _data==0); }
		bool valid() const { return (isFile() || _data); }
		void* getDataPtr() { return _data; }
		const std::string& getFileName() const { return _filename; }
		
		void setVolume(float v) { _volume = v; }
		float getVolume() const { return _volume; }
		
		void setBalance(float v) { _balance = v; }
		float getBalance() const { return _balance; }
        
        void setPitch(float v) { _pitch = v; }
        float getPitch() const { return _pitch; }
		
		void setLooping(bool b) { _looping = b; }
		bool getLooping() const  { return _looping; }
		
		void setAutoPlay(bool b) { _autoPlay = b; }
		bool getAutoPlay() const { return _autoPlay; }
		
		void setStreamed(bool b) { _streamed = b; }
		bool getStreamed() const { return _streamed; }
	
		 
	private:
		std::string _filename;
		void*		_data;
		float		_volume, _balance, _pitch;
		bool		_looping, _autoPlay, _streamed;
	};


	class Implementation : public osg::Referenced {
	public:
		Implementation() : osg::Referenced(), _sound(NULL), _errorCode(0) {}
		inline void setSound(Sound* sound) { _sound = sound; }
		virtual void open(const Source& source) = 0;
		virtual bool play() = 0;
		virtual bool pause() = 0;
		virtual bool stop() = 0;
		virtual void setVolume(float volume) = 0;
		virtual void setBalance(float balance) = 0;
        virtual void setPitch(float pitch) = 0;
		virtual double getDuration() = 0;
		virtual double getCurrentTime() = 0;
		virtual void setLooping(bool doLoop) = 0;
		virtual void seek(double t) = 0;
		virtual bool isPlaying() = 0;
		inline int getError() const { return _errorCode; }
		virtual bool valid() const = 0;
		virtual ~Implementation() {}
	protected:
		void setErrorCode(int err) { _errorCode = err; }
		Sound* _sound;
	private:
		int		_errorCode;
	};
	
	
protected:	
	Sound(Implementation* impl, const Source& source) : osg::Referenced(), _impl(impl), _source(source), _isPlaying(false) { impl->setSound(this); }
	
	inline bool open() 
	{
		_currentTime = 0;
		_impl->open(_source);
		_duration = _impl->getDuration();
		_impl->setLooping(_source.getLooping());
		_impl->setVolume(_source.getVolume());
		_impl->setBalance(_source.getBalance());
		if (_source.getAutoPlay()) {
			play();
		}
		
		return valid();
	}
	
	
	inline void close() {
		pause();
		stop();
		_impl = NULL; 
	}

public:	
	bool play();
	
	inline bool pause() {
		if (!valid()) return false;
		if (_isPlaying) {
			if (_impl->pause()) _isPlaying = false;
		} 
		return (_isPlaying == false);
	}
	
	inline void stop() {
		if (!valid()) return;
		pause();
		_impl->stop();
		seek(0);
	}
	
	inline const double& getCurrentTime()
	{
		if (valid() && _isPlaying)
			_currentTime = _impl->getCurrentTime();
		return _currentTime;
	}
	inline void seek(double p) {
		if (valid()) _impl->seek(p); 
	}
	
	~Sound() {
		close();
		_impl = NULL;
	}
	
	inline void setVolume(float f) {
		_source.setVolume(f);
		if (valid()) {
			_impl->setVolume(f); 
		}
	}
	
	inline float getVolume() const { return _source.getVolume(); }
	
	void setBalance(float f) {
		_source.setBalance(f);
		if(valid()) {
			_impl->setBalance(f);
		}
	}
	
	inline float getBalance() const { return _source.getBalance(); }
    
    void setPitch(float f) {
        _source.setPitch(f);
        if (valid()) {
            _impl->setPitch(f); 
        }
    }
    inline float getPitch() const { return _source.getPitch(); }
	
	inline const std::string& getFileName() const { return _source.getFileName(); } 
	
	inline bool valid() const { return (_impl.valid() && _impl->valid() && _source.valid()); }
	
	inline const double& getDuration() const { return _duration; }
	inline bool isPlaying() const { return _isPlaying; }
	
	void setLooping(bool b) { _source.setLooping(b); _impl->setLooping(b); }
	bool isLooping() const { return _source.getLooping(); }
    
    const Source& getSource() const { return _source; }
		
protected:
	void checkPlayingState() { if (valid()) _isPlaying = _impl->isPlaying(); }
	
private:
	osg::ref_ptr<Implementation>	_impl;
	Source							_source;
	bool							_isPlaying;
	double							_currentTime, _duration;
friend class SoundManager;
friend class SoundStateObserverThread;

};


}

#endif