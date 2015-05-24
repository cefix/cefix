/*
 *  VideoMediaManager.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 02.05.06.
 *  Copyright 2006 digitalmind.de // Stephan Maximilian Huber. All rights reserved.
 *
 */

#include <cefix/VideoMediaManager.h>
#include <cefix/SequenceGrabberManager.h>
#include <cefix/Settings.h>
#include <cefix/Export.h>

#ifdef CEFIX_FOR_IPHONE 

namespace cefix {

void initQuicktime(bool f=false) {}
}

#else
    #include <cefix/QTUtils.h>
#endif



namespace cefix {

/** 
 * implementation of Nodecallback, which calls the idle-method of the VideoMediaManager-Manager.
 * add an instance of this callback to your scene, and all grabbers are actualized
 */
 
class IdleCallback: public osg::NodeCallback {
    private:
        VideoMediaManager*      _qtmm;
        unsigned int                _previousTraversalNumber;
        osg::Node*                  _node;
        
    public:
        /// default constructor
        IdleCallback(osg::Node* node) : _node(node) {
            _qtmm = VideoMediaManager::instance();
        }
        /// default destructor
        virtual ~IdleCallback() {}

        /// implementation of the callback, calls the idle-method of the SequenceGrabberManager
        virtual void operator() (osg::Node* node, osg::NodeVisitor* nv) {         
            if (nv) {
                if ((_node == node) && (nv->getTraversalNumber()!=_previousTraversalNumber)){ 
                    _qtmm->idle();
                    _previousTraversalNumber = nv->getTraversalNumber();
                }
            }
            // must continue subgraph traversal.
            traverse(node,nv);
        }
};





VideoMediaManager* VideoMediaManager::instance() {
    static osg::ref_ptr< VideoMediaManager > s_qt_media_manager = new VideoMediaManager();
    
    return s_qt_media_manager.get();
}

VideoMediaManager::VideoMediaManager() 
{
    _currentlyClearing = false;
    _destructed = false;
    _debug = false;
    _extendedDebug = false;
    _threadedIdle = Settings::instance()->useThreadedMediaIdling();
    
    #if defined(CEFIX_QUICKTIME_AVAILABLE)
        initQuicktime();
    #endif
    
    if (_threadedIdle) 
    {
        _idleThread = new IdleThread(this);
        _idleThread->start();
    }

}

VideoMediaManager::~VideoMediaManager() 
{
}

osg::NodeCallback* VideoMediaManager::createIdleCallback(osg::Node* node) {
    return new IdleCallback(node);
}

    
void VideoMediaManager::cleanup() {
    SequenceGrabberManager::instance()->cleanup();
    if (_threadedIdle) 
    {
        _idleThread->stop();
        _idleThread->join();
    }
    _currentlyClearing = true;
    _mutex.lock();
    osg::notify(osg::INFO) << "VideoMediaManager: cleaning " << _media.size() << " items" <<  std::endl;
    
    _media.clear();
    _mutex.unlock();
    _currentlyClearing = false;
    #if defined(CEFIX_QUICKTIME_AVAILABLE)
        initQuicktime(true);
    #endif
    
    _destructed = true;
}

}