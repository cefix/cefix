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

#ifndef CEFIX_VIDEO_HEADER
#define CEFIX_VIDEO_HEADER

#include <osg/ImageStream>

namespace cefix {

class CoreVideoAdapter;

class Video : public osg::ImageStream {

public:
    Video();
    
    enum CoreVideoUsage {
        CoreVideoUsageUnspecified = 0,
        UseCoreVideo,
        DontUseCoreVideo
    };
    
    virtual double getCurrentTime() const { return _currentTime; }
    
    virtual void jumpTo(double t) = 0;
    
    virtual void setMovieRate(double r) = 0;
    
    virtual double getMovieRate() const = 0;
    
    virtual bool isPlaying() const = 0;
    
    virtual bool valid() const = 0;
    
    virtual void 	setTimeMultiplier (double r) { setMovieRate(r); }
    virtual double 	getTimeMultiplier () const { return getMovieRate(); }
    
    virtual void load(const std::string& file_name) = 0;
    
    virtual void deactivate() = 0;
    virtual void activate() = 0;
    virtual bool isActive() const = 0;
    
    virtual void 	setVolume (float) = 0;
    virtual float 	getVolume () const = 0;
    
    /** @return the current balance-setting (0 = neutral, -1 = left, 1 = right */
    virtual float getAudioBalance() = 0;
    /** sets the current balance-setting (0 = neutral, -1 = left, 1 = right */
    virtual void setAudioBalance(float b) = 0;
    
    virtual float getVideoWidth() { return s(); }
    virtual float getVideoHeight() { return t(); }
        
    /// Rewind stream to beginning.
    virtual void rewind() { jumpTo(0); }
    
    /// forward stream to the end
    virtual void forward() { jumpTo(getLength()); }
    
    void setCoreVideoUsage(CoreVideoUsage cvu) { _coreVideoUsage = cvu; }
    CoreVideoUsage getCoreVideoUsage() const { return _coreVideoUsage; }
    
    virtual void setCoreVideoAdapter(CoreVideoAdapter* adapter) { _coreVideoAdapter = adapter; }
    CoreVideoAdapter* getCoreVideoAdapter() { return _coreVideoAdapter; }
    
    static osg::Texture* createTexture(osg::Image* img, bool create_texture_rectangle = false);
    
    class DimensionChangedCallback : public osg::Referenced {
    public:
        virtual void operator()(Video* video) = 0;
    };
    
    typedef std::vector< osg::ref_ptr<DimensionChangedCallback> > DimensionChangedCallbacks;
    
    void addDimensionChangedCallback(DimensionChangedCallback* cb) { _dimensionChangedCallbacks.push_back(cb); }
    
    bool removeDimensionChangedCallback(DimensionChangedCallback* cb) {
        DimensionChangedCallbacks::iterator itr = std::find(_dimensionChangedCallbacks.begin(), _dimensionChangedCallbacks.end(), cb);
        if (itr != _dimensionChangedCallbacks.end()) {
            _dimensionChangedCallbacks.erase(itr);
            return true;
        }
        return false;
    }
    
protected:
    void handleDimensionChangedCallbacks();
    
    virtual osg::Texture* createTexture(bool create_texture_rectangle);
    void setCurrentTime(double t) const { _currentTime = t; }
    
    mutable double _currentTime;
    CoreVideoAdapter* _coreVideoAdapter;
    CoreVideoUsage _coreVideoUsage;
    DimensionChangedCallbacks _dimensionChangedCallbacks;
};

}

#endif