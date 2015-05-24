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

#ifndef VIDEO_NEW_FRAME_CALLBACK_HEADER
#define VIDEO_NEW_FRAME_CALLBACK_HEADER

#include <osg/Referenced>
#include <osg/Image>

namespace cefix{

/** base callback class, fires, when a new frame is available */
class VideoNewFrameCallback : public virtual osg::Referenced {

	public:
	
		VideoNewFrameCallback() : osg::Referenced() {}
		
		virtual void operator()(osg::Image* image) = 0;
		
};

class VideoFrameCallbackHandlerBase {
public:
    typedef std::list< osg::ref_ptr<VideoNewFrameCallback> > VideoNewFrameCallbackList;
    
    VideoFrameCallbackHandlerBase() : _callbacks() {}
    
    void addNewFrameCallback(VideoNewFrameCallback* cb) { _callbacks.push_back(cb); }
    void removeNewFrameCallback(VideoNewFrameCallback* cb) { _callbacks.remove(cb); }

    inline void traverseCallbacks(osg::Image* image) {
        for(VideoNewFrameCallbackList::iterator i = _callbacks.begin(); i != _callbacks.end(); ++i) 
        {
            (*i)->operator()(image); 
        }
    }
    
    void clearNewFrameCallbacks() { _callbacks.clear(); }
    const VideoNewFrameCallbackList& getNewFrameCallbacks() const { return _callbacks; }
    VideoNewFrameCallbackList& getNewFrameCallbacks() { return _callbacks; }
    
    virtual ~VideoFrameCallbackHandlerBase() {}
    
protected:
    VideoNewFrameCallbackList _callbacks;

};


class NestedVideoNewFrameCallback : public VideoNewFrameCallback, public VideoFrameCallbackHandlerBase {

public:
        	
    NestedVideoNewFrameCallback() : VideoNewFrameCallback(), VideoFrameCallbackHandlerBase() {}

};

}

#endif