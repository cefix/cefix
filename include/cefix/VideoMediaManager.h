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

#ifndef VIDEO_MEDIA_MANAGER_HEADER_
#define VIDEO_MEDIA_MANAGER_HEADER_

#include <vector>
#include <list>
#include <OpenThreads/Thread>
#include <OpenThreads/Block>
#include <osg/ref_ptr>
#include <osg/observer_ptr>
#include <osg/Referenced>
#include <osg/NodeCallback>
#include <cefix/VideoMedia.h>
#include <osg/Timer>
#include <OpenThreads/Thread>
#include <OpenThreads/Block>
#include <iostream>

namespace cefix {
    
    /** this class provides an interface for various Video-Media-types, the get registered with this class and the 
     * class call their idle-methods on a regular basis
     */
    class CEFIX_EXPORT VideoMediaManager : public osg::Referenced {
    
        protected:
            class IdleThread : public OpenThreads::Thread, public osg::Referenced {
            
            public:
                IdleThread(VideoMediaManager* vmm) : OpenThreads::Thread(), osg::Referenced(), _vmm(vmm) {}
                
                virtual void run()
                {
                    _finished = false;
                    while (!_finished) 
                    {
                        _vmm->idleImplementation();
                        _block.block();
                    }
                }
                
                void stop() 
                { 
                    _finished = true; 
                    go(); 
                }
                
                void go() 
                { 
                    _block.release(); 
                }
            private:
                VideoMediaManager*      _vmm;
                bool                    _finished;
                OpenThreads::Block      _block;
            };

            
            typedef std::list<osg::observer_ptr< VideoMedia > > MediaVector;
            MediaVector         _media;
            bool                _currentlyClearing;
            OpenThreads::Mutex  _mutex;
			osg::Timer_t		_lastMessage;
			bool				_debug, _extendedDebug;
			bool				_destructed;
            bool                _threadedIdle;
            osg::ref_ptr<IdleThread> _idleThread;
        
        public:
    
            /// default constructor
            VideoMediaManager();
            
            /// default destructor
            virtual ~VideoMediaManager();
            /// do quicktime-related cleanup, call it before exiting the application
            void cleanup();
            
            /// @return the singleton instance of this class
            static VideoMediaManager* instance();
            
            inline void idle() 
            { 
                if (!_threadedIdle) 
                    idleImplementation();
                else 
                    _idleThread->go(); 
            }
            
            /// calls all registered media's idle-method, so they get a chance to update themselves
            void idleImplementation() {
                _mutex.lock();
				unsigned int running = 0;
                for(MediaVector::iterator i = _media.begin(); i!= _media.end(); ++i) 
                if ((*i).valid() && (*i)->isRunning()) {
					running++;
					//nur laufende prozesse updaten 
                    (*i)->idle();
				}
				if ((_debug) && (osg::Timer::instance()->delta_s(_lastMessage, osg::Timer::instance()->tick()) >= 1.0f)) {
					std::cout << "VideoMediaManager:: active items: " << running << " all items: " << _media.size() << std::endl;
					_lastMessage = osg::Timer::instance()->tick();
					if (_extendedDebug) {
						for(MediaVector::iterator i = _media.begin(); i!= _media.end(); ++i) 
							if ((*i).valid()) {
								std::cout << " -> " << (*i)->getMediaTypeName() << std::endl;
							}
						
					}
				}
                _mutex.unlock();
            }
            
            /// @return a nodecallback, which you can attach to a geode of your scenegrap
            osg::NodeCallback* createIdleCallback(osg::Node* node);
            
            /// adds a media to the list of medias. 
            /// You should not call this function, your media register itself with the manager
            /// @param media the media to register
            void add(cefix::VideoMedia* media) { 
				if (_destructed) return;
                _mutex.lock();
                _media.push_back(media);
                _mutex.unlock();
                
            }
            
            /// removes a media from the list of medias. 
            /// You should not call this function, your media removes itself from the manager
            /// @param media the media to register            
            void remove(VideoMedia* media) {
				if (_currentlyClearing || _destructed) return;
                _mutex.lock();
				std::vector<MediaVector::iterator> to_remove;
                for(MediaVector::iterator i = _media.begin(); i!= _media.end(); ) {
                    if ((!(*i).valid()) || (*i)->ID() == media->ID()) {
                       i =  _media.erase(i);
                    }
					else 
						++i;
                }
                _mutex.unlock();
                
            }
			
			void setDebugFlag(bool flag, bool extendedOutput = false) { 
				_debug = flag; 
				_extendedDebug = extendedOutput; 
				_lastMessage = osg::Timer::instance()->tick(); 
			}
    };
            
}

#endif
